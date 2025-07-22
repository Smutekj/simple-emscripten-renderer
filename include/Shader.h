#pragma once

#include "IncludesGl.h"
#include "ShaderLoader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <type_traits>
#include <variant>
#include <filesystem>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/type_ptr.hpp>

class ShaderHolder;

//! \struct TextureGlData
//! \brief combines slot in Sprite + GL handle
struct TextureGlData
{
    int slot = 0;
    GLuint handle = 0;
};

//! Types used in GLSL for uniforms
using UniformType = std::variant<float, bool, int, glm::vec4, glm::vec3, glm::vec2>;

//! \struct VariablesData
//! \brief contains mappings from uniform names of uniforms and textures in corresponding shader
struct VariablesData
{
    std::unordered_map<std::string, UniformType> uniforms;
    std::unordered_map<std::string, TextureGlData> textures;

    std::string setTexture(int slot, GLuint handle);
};

//! \class Shader
//! \brief combines vertex + framgnet shader and holds uniform info of the fragment shader
//!  the shaders are load via specified paths
class Shader
{

public:
    Shader() = default;
    Shader(const std::string &vertex_shader_code, const std::string &frament_shader_code);
    Shader(const std::filesystem::path &vertex_path, const std::filesystem::path &fragment_path);
    Shader(const std::filesystem::path &vertex_path, const std::filesystem::path &fragment_path, const std::string &shader_name);

    ~Shader();
    Shader(const Shader& other) = default;
    Shader(Shader&& other) = default;
    Shader& operator=(const Shader& other) = default;
    Shader& operator=(Shader&& other) = default;

    VariablesData &getVariables();
    GLuint getId() const;
    void recompile();
    bool loadFromCode(const std::string &vertex_code, const std::string &fragment_code);

    const std::string &getFragmentPath();
    const std::string &getVertexPath();

    // use/activate the shader
    void use();

    // utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float x, float y) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

    void setUniforms();

    template <class ValueType>
    constexpr void setUniform(const std::string &name, const ValueType &value);

    const std::string &getName() const;

    void activateTexture(TextureArray handles);

    void setUniform2(const std::string &uniform_name, UniformType uniform_value);

    void saveUniformValue(const std::string &uniform_name, UniformType uniform_value);

    void setReloadOnChange(bool new_flag_value);

    bool getReloadOnChange() const;
    bool wasSuccessfullyBuilt()const;

    friend ShaderHolder;

private:
    void retrieveCode(const char *code_path, std::string &code);

    unsigned int m_id = 0; //!< OpenGL id of the program
    std::string m_vertex_path;
    std::string m_fragment_path;
    std::string m_shader_name = "default_name";       //!< shader_id
    std::filesystem::file_time_type m_last_writetime; //!< last time of change of the fragment shader file.

    bool m_reload_on_file_change = false; //!< reloads the shader when it is changed in filesystem (this is useful for playing with shaders, but slow because of filesystem calls)
    bool m_successfully_built = false; //!< is set to true if built process runs successfully, when false the shader is not used 

    VariablesData m_variables; //!< contains data about uniforms and textures in the fragment shader.

public:
    inline static float m_time;
};

struct ShaderUIData
{

    ShaderUIData(Shader &program);

    Shader *p_program = nullptr;
    std::string filename = "";
    VariablesData &variables;
    std::filesystem::file_time_type last_write_time;
};

//! \class ShaderHolder
//! \brief holds shaders themselves and also data about uniforms and textures in them
class ShaderHolder
{

    using ShaderMap = std::unordered_map<std::string, std::shared_ptr<Shader>>;
    using ShaderUIDataMap = std::unordered_map<std::string, ShaderUIData>;

public:
    ShaderHolder();
    explicit ShaderHolder(std::filesystem::path resources_path);

    ~ShaderHolder() = default;
    ShaderHolder(const ShaderHolder&) = default;
    ShaderHolder(ShaderHolder&&) = default;
    ShaderHolder& operator=(const ShaderHolder&) = default;
    ShaderHolder& operator=(ShaderHolder&&) = default;

    Shader &get(const std::string &id);

    void use(const std::string &id);

    bool load(const std::string &name, const std::string &vertex_filename, const std::string &fragment_filename);
    bool loadFromCode(const std::string &id, const std::string &vertex_code, const std::string &fragment_code);

    void erase(const std::string &shader_id);

    const ShaderMap &getShaders() const;
    ShaderUIData &getData(const std::string &name);

    bool contains(const std::string &shader_id) const;

    ShaderHolder::ShaderUIDataMap &getAllData();

    void refresh();

    void initializeUniforms();

    bool setBaseDirectory(std::filesystem::path dir);

private:
    ShaderMap m_shaders;
    ShaderUIDataMap m_shader_data;

    std::filesystem::path m_resources_path;
};

std::vector<std::string> inline separateLine(std::string line, char delimiter = ' ');

inline std::string trim(const std::string &input);

inline bool replace(std::string &str, const std::string &from, const std::string &to);

inline UniformType extractValue(std::string type_string, std::string initial_value);

