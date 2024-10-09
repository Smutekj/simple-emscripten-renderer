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

#include <magic_enum.hpp>
#include <magic_enum_utility.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/type_ptr.hpp>

//! helper type for the visitor (stolen from cpp_reference)
template <class... Ts>
struct overloads : Ts...
{
    using Ts::operator()...;
};

class ShaderHolder;

//! \struct slot in Sprite + GL handle
struct TextureGlData
{
    int slot = 0;
    GLuint handle = 0;
};

//! Types used in GLSL for uniforms
using UniformType = std::variant<float, bool, int, glm::vec4, glm::vec3, glm::vec2>;

//! \struct contains mappings from uniform names of uniforms and textures in corresponding shader
struct VariablesData
{
    std::unordered_map<std::string, UniformType> uniforms;
    std::unordered_map<std::string, TextureGlData> textures;

    std::string setTexture(int slot, GLuint handle);
};

//! \class contains all the data
//! \brief combines vertex + framgnet shader
//! \brief the shaders are load via specified paths
//! \brief also contains uniforms and textures present in the FragmentShader
class Shader
{

public:
    Shader() = default;
    Shader(const std::string vertex_path, const std::string fragment_path);
    Shader(const std::string vertex_path, const std::string fragment_path, std::string shader_name);

    VariablesData &getVariables();
    GLuint getId() const;
    void recompile();
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

    void activateTexture(std::array<GLuint, 2> handles);

public:
    void setUniform2(std::string uniform_name, UniformType uniform_value);

    void saveUniformValue(std::string uniform_name, UniformType uniform_value);

    friend ShaderHolder;

private:
    void retrieveCode(const char *code_path, std::string &code);

    unsigned int m_id = -1; // the program ID
    std::string m_fragment_path;
    std::string m_vertex_path;
    std::string m_shader_name = "default_name";
    std::filesystem::file_time_type m_last_writetime;

    VariablesData m_variables;

public:
    inline static float m_time;
};

//! \struct Not sure?
struct ShaderUIData
{

    ShaderUIData(Shader &program);

    Shader *p_program = nullptr;
    std::string filename = "";
    VariablesData &variables;
    std::filesystem::file_time_type last_write_time;
};

//! \class holds shaders themselves and also data about uniforms and textures in them
class ShaderHolder
{

public:
    Shader &get(std::string id);

    void use(std::string id);

    void load(std::string name, std::string vertex_path, std::string fragment_path);

    void erase(const std::string &shader_id);

    const std::unordered_map<std::string, std::unique_ptr<Shader>> &getShaders() const;

    bool contains(const std::string &shader_id) const;

    ShaderUIData &getData(std::string name);

    auto &getAllData();

    void refresh();

    void initializeUniforms();

private:
    std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;
    std::unordered_map<std::string, ShaderUIData> m_shader_data;
};

std::vector<std::string> inline separateLine(std::string line, char delimiter = ' ');

inline std::string trim(std::string input);

inline bool replace(std::string &str, const std::string &from, const std::string &to);

inline UniformType extractValue(std::string type_string, std::string initial_value);