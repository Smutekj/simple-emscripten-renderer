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

#include "Color.h"

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
using UniformType = std::variant<float, bool, int, glm::vec4, glm::vec3, glm::vec2, glm::mat4, Color>;

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

enum class ShaderType
{
    Vertex,
    Fragment,
    Geometry,
    TessControl,
    TessEvaluation,
    Compute,
};

GLenum getGLCode(ShaderType type)
{

    using st = ShaderType;
    switch (type)
    {
    case st::Vertex:
        return GL_VERTEX_SHADER;
    case st::Fragment:
        return GL_FRAGMENT_SHADER;
    case st::Geometry:
        return GL_GEOMETRY_SHADER;
    case st::TessControl:
        return GL_TESS_CONTROL_SHADER;
    case st::TessEvaluation:
        return GL_TESS_EVALUATION_SHADER;
    case st::Compute:
        return GL_COMPUTE_SHADER;
    };
    return 0;
}
std::string getShaderTypeName(ShaderType type)
{
    using st = ShaderType;
    switch (type)
    {
    case st::Vertex:
        return "Vertex";
    case st::Fragment:
        return "Fragment";
    case st::Geometry:
        return "Geometry";
    case st::TessControl:
        return "Tesselation Control";
    case st::TessEvaluation:
        return "Tesselation Evaluation";
    case st::Compute:
        return "Compute";
    };
    return "";
}

class Shader2
{

public:
    explicit Shader2(const std::string &source_code, ShaderType type);
    explicit Shader2(const std::filesystem::path &code_path, ShaderType type);
    ~Shader2();

    GLuint getId() const;

    bool isCompiled() const;
    bool compile(const std::string &source_code);

private:
    bool m_compiled = false;
    ShaderType m_type;
    GLuint m_id = 0;
};

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    void pushShader(const std::string &shader_code, ShaderType type);
    void linkShaders();

    void use() const;

    bool isValid() const;

    void setUniform(const std::string &uniform_name, UniformType value);
    VariablesData &getUniforms();

private:
    void updateUniforms();

private:
    bool m_valid = false;
    bool m_linked = false;
    VariablesData m_uniforms;
    GLuint m_id = 0;
};

/* class ShaderProgramHolder
{
    void addShader()
    {

    }

    void refresh()
    {

    }

private:
    struct ShaderFileData
    {
        std::string shader_id;
        std::filesystem::file_time_type last_write_time;
        std::filesystem::path path;
    };

    std::unordered_map<std::string, std::unordered_map<ShaderType, ShaderFileData>> m_file_info;
    std::unordered_map<std::string, ShaderProgram> m_shaders;
} */;