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
//!  TODO:  use std::filesystem::path instead of strings like a retard
class Shader
{

public:
    Shader() = default;
    Shader(const std::string &vertex_path, const std::string &fragment_path);
    Shader(const std::string &vertex_path, const std::string &fragment_path, const std::string &shader_name);

    VariablesData &getVariables();
    GLuint getId() const;
    
    void recompile();
    void recompile(const char* fragment_text);

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

    void setUniform2(const std::string &uniform_name, UniformType uniform_value);

    void saveUniformValue(const std::string &uniform_name, UniformType uniform_value);


    std::filesystem::file_time_type getLastChangeTime() const;

private:
    void retrieveCode(const char *code_path, std::string &code);

    unsigned int m_id = -1; //!< OpenGL id of the program
    std::string m_vertex_path;
    std::string m_fragment_path;
    std::string m_shader_name = "default_name";       //!< shader_id
    std::filesystem::file_time_type m_last_writetime; //!< last time of change of the fragment shader file.

    VariablesData m_variables; //!< contains data about uniforms and textures in the fragment shader.

public:
    inline static float m_time;
    
    friend ShaderHolder;
};



//! \class ShaderHolder
//! \brief holds shaders themselves and also data about uniforms and textures in them
class ShaderHolder
{

    using ShaderMap = std::unordered_map<std::string, std::unique_ptr<Shader>>;

public:
    Shader &get(const std::string &id);

    void use(const std::string &id);

    void load(const std::string &name, const std::string &vertex_filename, const std::string &fragment_filename);

    void erase(const std::string &shader_id);

    const ShaderMap &getShaders() const;

    bool contains(const std::string &shader_id) const;

    void refresh();

    void initializeUniforms();

private:
    ShaderMap m_shaders;
};



std::vector<std::string> inline separateLine(std::string line, char delimiter = ' ');

inline std::string trim(std::string input);

inline bool replace(std::string &str, const std::string &from, const std::string &to);

inline UniformType extractValue(std::string type_string, std::string initial_value);

//! helper type for the visitor (stolen from cpp_reference)
template <class... Ts>
struct overloads : Ts...
{
    using Ts::operator()...;
};