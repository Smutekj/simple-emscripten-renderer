#pragma once

#include "IncludesGl.h"
#include "GLTypeDefs.h"

#include <string>
#include <fstream>
#include <sstream>
#include <numeric>
#include <unordered_map>
#include <type_traits>
#include <variant>
#include <filesystem>


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class ShaderHolder;

//! \struct TextureGlData
//! \brief combines slot in Sprite + GL handle
struct TextureGlData
{
    int slot = 0;
    GLuint handle = 0;
    bool needs_update = true;
};

//! Types used in GLSL for uniforms
using UniformType = std::variant<float, bool, int, glm::vec4, glm::vec3, glm::vec2, glm::mat4>;

//! \struct VariablesData
//! \brief contains mappings from uniform names of uniforms and textures in corresponding shader
struct VariablesData
{
    struct UniformValue
    {
        UniformType value;
        bool needs_update = true;
    };
    std::unordered_map<std::string, UniformValue> uniforms;
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
    Shader(const Shader &other) = default;
    Shader(Shader &&other) = default;
    Shader &operator=(const Shader &other) = default;
    Shader &operator=(Shader &&other) = default;

    VariablesData &getVariables();
    GLuint getId() const;
    void recompile();
    bool loadFromCode(const std::string &vertex_code, const std::string &fragment_code);

    const std::string &getFragmentPath();
    const std::string &getVertexPath();

    // use/activate the shader
    void use();

    const std::string &getName() const;
    bool wasSuccessfullyBuilt() const;

    void setTexture(TextureArray handles);
    void setTexture(const std::string &uniform_tex_key, int slot, GLuint tex_handle);
    void setUniform(const std::string &uniform_name, UniformType uniform_value);

    void setReloadOnChange(bool new_flag_value);
    bool getReloadOnChange() const;

    friend ShaderHolder;

private:
    template <class ValueType>
    constexpr void updateUniform(const std::string &name, const ValueType &value);
    void updateUniforms();

    void retrieveCode(const char *code_path, std::string &code);

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

private:
    unsigned int m_id = 0; //!< OpenGL id of the program
    std::string m_vertex_path;
    std::string m_fragment_path;
    std::string m_shader_name = "default_name";       //!< shader_id
    std::filesystem::file_time_type m_last_writetime; //!< last time of change of the fragment shader file.

    bool m_reload_on_file_change = false; //!< reloads the shader when it is changed in filesystem (this is useful for playing with shaders, but slow because of filesystem calls)
    bool m_successfully_built = false;    //!< is set to true if built process runs successfully, when false the shader is not used

    VariablesData m_variables; //!< contains data about uniforms and textures in the fragment shader.

public:
    inline static float m_time;
};


std::vector<std::string> inline separateLine(std::string line, char delimiter = ' ');

inline std::string trim(const std::string &input);

inline bool replace(std::string &str, const std::string &from, const std::string &to);

inline UniformType extractValue(std::string type_string, std::string initial_value);
