#include "Shader.h"

#include "ShaderLoader.h"

#include <SDL2/SDL.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>

std::string loadFileToString(std::string path)
{
    SDL_RWops *rw = SDL_RWFromFile(path.c_str(), "rb");
    if (!rw)
    {
        SDL_Log("Failed to open %s: %s", path.c_str(), SDL_GetError());
        return {};
    }

    Sint64 size = SDL_RWsize(rw);
    std::string buffer(size, '\0');
    SDL_RWread(rw, buffer.data(), 1, size);
    SDL_RWclose(rw);

    return buffer;
}

std::istringstream loadFileToStream(std::string path)
{
    return std::istringstream{loadFileToString(path)};
}

//! \brief read a shader file in \p filename and extracts all uniforms (that are not textures!)
//! \brief the uniform names-values pairs are stored in \p shader_data
//! \param shader_data
//! \param filename
void static extractUniformNamesFromCode(VariablesData &shader_data, const std::string &code)
{
    auto lines = separateLine(code, '\n');
    for (auto &line : lines)
    {
        auto split_line = separateLine(line, ' ');
        if (split_line.size() < 3)
        {
            continue;
        } //! try only lines that have some words on them
        if (split_line[0] == "uniform")
        {
            if (split_line[1] == "sampler2D") //! skip textures, we do them separately
            {
                continue;
            }
            auto initial_value = separateLine(line, '=');
            std::string initial_value_string = "";
            //! if there is no '=' then there is no initial value
            initial_value_string = initial_value.size() > 1 ? initial_value[1] : "";

            std::string uniform_name = split_line[2];
            const auto &type_string = split_line[1];
            auto value = extractValue(type_string, initial_value_string);
            //! remove ; at the end
            if (uniform_name.back() == ';')
            {
                uniform_name.pop_back();
            }
            shader_data.uniforms[uniform_name] = {value, true};
        }
    }
}

//! \brief removes initial values of uniforms from shader code
//!         This is necessary because
//! \param code       string containing old shader code
//! \returns new string with reduced code
std::string removeInitialValues(const std::string &code)
{
    std::string new_code;
    auto lines = separateLine(code, '\n');
    for (auto &line : lines)
    {
        //! remove intial values
        if (line.find("uniform") != std::string::npos && line.find("=") != std::string::npos)
        {
            line = line.substr(0, line.find("=")) += ";";
        }
        new_code += line + "\n";
    }
    return new_code;
}

//! \brief read a shader file in \p filename and extracts all uniforms (that are not textures!)
//! \brief the uniform names-values pairs are stored in \p shader_data
//! \param shader_data
//! \param filename
void static extractTextureNamesFromCode(VariablesData &shader_data, const std::string &code)
{
    auto lines = separateLine(code, '\n');
    int slot = 0;
    for (auto &line : lines)
    {
        auto split_line = separateLine(line, ' ');
        if (split_line.size() < 3)
        {
            continue;
        } //! try only lines that have some words on them
        if (split_line[0] == "uniform" && split_line[1] == "sampler2D")
        {

            std::string texture_var_name = split_line[2];

            //! remove ; at the end
            auto colon_pos = texture_var_name.find_last_of(';');
            texture_var_name = texture_var_name.substr(0, colon_pos);
            // shader_data.p_program->setUniform2(texture_var_name, texture_shader_id);
            shader_data.textures[texture_var_name] = {slot, 0};
            slot++;
        }
    }
}
//! \brief read a shader file in \p filename and extracts all uniforms (that are not textures!)
//! \brief the uniform names-values pairs are stored in \p shader_data
//! \param shader_data
//! \param filename
void extractUniformNames(VariablesData &shader_data, const std::string &filename)
{
    auto file = loadFileToStream(filename);
    if (!file.good())
    {
        throw std::runtime_error("File not found: " + filename);
    }

    shader_data.uniforms.clear();

    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream iss(line);
        auto split_line = separateLine(line, ' ');
        if (split_line.size() < 3)
        {
            continue;
        } //! try only lines that have some words on them
        if (split_line[0] == "uniform")
        {
            if (split_line[1] == "sampler2D")
            {
                continue;
            }
            auto initial_value = separateLine(line, '=');
            std::string initial_value_string = "";
            initial_value_string = initial_value.size() > 1 ? initial_value[1] : "";

            std::string uniform_name = split_line[2];
            const auto &type_string = split_line[1];
            auto value = extractValue(type_string, initial_value_string);
            //! remove ; at the end
            if (uniform_name.back() == ';')
            {
                uniform_name.pop_back();
            }
            shader_data.uniforms[uniform_name] = {value, true};
        }
    }
}

//! \brief read a shader file in \p filename and extracts all uniforms that are textures
//! \brief the texture names-values pairs are stored in \p shader_data
//! \param shader_data
//! \param filename
[[maybe_unused]] void static extractTextureNames(VariablesData &shader_data, std::string filename)
{
    auto file = loadFileToStream(filename);
    if (!file.good())
    {
        throw std::runtime_error("File not found: " + filename);
    }

    int slot = 0;
    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream iss(line);
        auto split_line = separateLine(line, ' ');
        if (split_line.size() < 3) //! try only lines that have some words on them
        {
            continue;
        }
        if (split_line[0] == "uniform" && split_line[1] == "sampler2D")
        {

            std::string texture_var_name = split_line[2];

            //! remove ; at the end
            auto colon_pos = texture_var_name.find_last_of(';');
            texture_var_name = texture_var_name.substr(0, colon_pos);
            // shader_data.p_program->setUniform2(texture_var_name, texture_shader_id);
            shader_data.textures[texture_var_name] = {slot, 0};
            slot++;
        }
    }
}
//! utility uniform functions
void setBool(GLuint id, const std::string &name, bool value)
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void setInt(GLuint id, const std::string &name, int value)
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
};
// ------------------------------------------------------------------------
void setFloat(GLuint id, const std::string &name, float value)
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
};
// ------------------------------------------------------------------------
void setVec2(GLuint id, const std::string &name, const glm::vec2 &value)
{
    glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}
// ------------------------------------------------------------------------
void setVec2(GLuint id, const std::string &name, float x, float y)
{
    glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void setVec3(GLuint id, const std::string &name, const glm::vec3 &value)
{
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}
// ------------------------------------------------------------------------
void setVec3(GLuint id, const std::string &name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void setVec4(GLuint id, const std::string &name, const glm::vec4 &value)
{
    glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}
// ------------------------------------------------------------------------
void setVec4(GLuint id, const std::string &name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void setMat2(GLuint id, const std::string &name, const glm::mat2 &mat)
{
    glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void setMat3(GLuint id, const std::string &name, const glm::mat3 &mat)
{
    glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void setMat4(GLuint id, const std::string &name, const glm::mat4 &mat)
{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
//! \brief connects a slot in shader with GL handle of the texture
//! \param slot   the slot where the texture will be bound
//! \param handle the GL handle of the texture
//! \returns name of the texture at the slot
std::string VariablesData::setTexture(int slot, GLuint handle)
{
    auto name_it = std::find_if(textures.begin(), textures.end(), [slot](auto &tex_data)
                                { return tex_data.second.slot == slot; });

    if (name_it != textures.end())
    {
        textures.at(name_it->first) = {slot, handle, true};
        return name_it->first;
    }
    return "";
}

//! \param name     name of the uniform
//! \param value    value of the uniform (the gl call is determined by the value type)
template <class ValueType>
constexpr void Shader::updateUniform(const std::string &name, const ValueType &value)
{
    if constexpr (std::is_same_v<ValueType, UniformType>)
    {
    }
    else if constexpr (std::is_same_v<ValueType, float>)
    {
        setFloat(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, bool>)
    {
        setBool(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, int>)
    {
        setInt(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::vec2>)
    {
        setVec2(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::vec3>)
    {
        setVec3(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::vec4>)
    {
        setVec4(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::mat2>)
    {
        setMat2(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::mat3>)
    {
        setMat3(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::mat4>)
    {
        setMat4(m_id, name, value);
    }
    else
    {
    }
}

const std::string &Shader::getName() const
{
    return m_shader_name;
}

bool Shader::wasSuccessfullyBuilt() const
{
    return m_successfully_built;
}

void Shader::setTexture(const std::string &uniform_tex_key, int slot, GLuint tex_handle)
{
    if (m_variables.textures.contains(uniform_tex_key))
    {
        m_variables.textures.at(uniform_tex_key) = {slot, tex_handle, true};
    }
}
//! \brief does GL calls to activate textures at the slots
void Shader::setTexture(TextureArray handles)
{
    for (size_t slot = 0; slot < handles.size(); ++slot)
    {
        if (handles.at(slot) != 0)
        {
            auto name = m_variables.setTexture(slot, handles.at(slot));
        }
    }
}

void Shader::setUniform(const std::string &uniform_name, UniformType uniform_value)
{
    m_variables.uniforms[uniform_name] = {uniform_value, true};
}

void Shader::retrieveCode(const char *code_path, std::string &code)
{
    std::ifstream shader_file;
    // ensure ifstream objects can throw exceptions:
    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {

        std::cout << code_path << "\n";
        // open files
        shader_file.open(code_path);
        if (!shader_file.is_open())
        {
            std::cout << "FILE NOT FOUND!\n";
        }
        else
        {
            std::cout << "FILE FOUND!\n";
        }

        std::stringstream shader_stream;
        // read file's buffer contents into streams
        shader_stream << shader_file.rdbuf();
        // close file handlers
        shader_file.close();
        // convert stream into string
        code = shader_stream.str();
        std::cout << code << "\n";
    }
    catch (std::ifstream::failure &e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
}

Shader::Shader(const std::string &vertex_shader_code, const std::string &frament_shader_code)
{
    if (!loadFromCode(vertex_shader_code, frament_shader_code))
    {
        return; //! shouldn't I use maybe some flag to test that the shader is ok?
    }
    extractUniformNamesFromCode(m_variables, frament_shader_code);
    extractTextureNamesFromCode(m_variables, frament_shader_code);
    m_shader_name = frament_shader_code;
}

//! \brief construct from paths to vertex and fragment shaders
Shader::Shader(const std::filesystem::path &vertex_path,
               const std::filesystem::path &fragment_path,
               const std::string &shader_name) : m_vertex_path(vertex_path.string()),
                                                 m_fragment_path(fragment_path.string()),
                                                 m_shader_name(shader_name)
{
    recompile();
}

Shader::Shader(const std::filesystem::path &vertex_path, const std::filesystem::path &fragment_path)
    : m_vertex_path(vertex_path.string()),
      m_fragment_path(fragment_path.string()),
      m_shader_name(fragment_path.string())
{
    recompile();
}

Shader::~Shader()
{
    if (m_id != 0)
    {
        glDeleteProgram(m_id);
    }
}

bool Shader::loadFromCode(const std::string &vertex_code, const std::string &fragment_code)
{
    m_successfully_built = false;

    try
    {
        extractTextureNamesFromCode(m_variables, fragment_code);
        extractUniformNamesFromCode(m_variables, fragment_code);
    }
    catch (std::exception &e)
    {
        std::cout << "failed to extract variales from code!" << std::endl;
        return false;
    }

    auto cleaned_fragment_code = removeInitialValues(fragment_code);

    const char *vShaderCode = vertex_code.c_str();
    const char *fShaderCode = cleaned_fragment_code.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        return false;
    };

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << "\n"
                  << "PROGRAM: " << m_fragment_path << "\n";
        return false;
    };

    // shader Program
    m_id = glCreateProgram();
    glAttachShader(m_id, vertex);
    glAttachShader(m_id, fragment);
    glLinkProgram(m_id);
    // print linking errors if any
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
        return false;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glCheckError();

    m_successfully_built = true;
    return true;
}

//! \brief extracts uniforms and textures then
//! \brief does all the GL calls to load the shader on the GPU
void Shader::recompile()
{
#if !defined(__ANDROID__)
    glsl_include::ShaderLoader vertex_loader = glsl_include::ShaderLoader("#include");
    glsl_include::ShaderLoader fragment_loader = glsl_include::ShaderLoader("#include");
    std::string vertex_code = vertex_loader.load_shader(m_vertex_path);
    std::string fragment_code = vertex_loader.load_shader(m_fragment_path);
#else
    std::string vertex_code = loadFileToString(m_vertex_path);
    std::string fragment_code = loadFileToString(m_fragment_path);
#endif
    loadFromCode(vertex_code, fragment_code);

    extractUniformNames(m_variables, getFragmentPath());
}

//! \brief calls glUseProgram(id)
//! \brief the shader is recompiled in case the fragment shaderfile
//! \brief  has been changed since the last time
void Shader::use()
{

    if (m_reload_on_file_change)
    {

        auto last_time = std::filesystem::last_write_time(m_fragment_path);
        if (last_time != m_last_writetime)
        {
            if (m_id != 0) //! 0 is the default value so it makes no sense to delete?
            {
                glDeleteProgram(m_id);
            }
            recompile();
            m_last_writetime = last_time;
        }
    }
    if (!m_successfully_built)
    {
        std::cout << "WARNING, Trying to use unbuilt shader named: " << m_shader_name << "\n";
        std::cout << "The shader will not be used!\n";
        return;
    }
    glUseProgram(m_id);
    glCheckErrorMsg("Error in Shader use");

    updateUniforms();
}

void Shader::updateUniforms()
{
    for (auto &[key, uniform] : m_variables.uniforms)
    {
        if (uniform.needs_update)
        {
            auto update_value = [&key, this](auto &&v)
            {
                using T = std::decay_t<decltype(v)>;
                updateUniform<T>(key, v);
            };
            glCheckErrorMsg((key + " does not exist in the shader").c_str());
            std::visit(update_value, uniform.value);

            uniform.needs_update = false;
        }
    }

    //! This is retarded, I should just have start-time attribute as part of vertex attributes or something...
    if (m_variables.uniforms.contains("u_time"))
    {
        m_variables.uniforms.at("u_time") = {Shader::m_time, true};
        updateUniform("u_time", Shader::m_time);
    }

    for (auto &[key, uniform_tex] : m_variables.textures)
    {
        if (uniform_tex.needs_update)
        {
            setUniform(key, uniform_tex.slot);
            uniform_tex.needs_update = false;
        }
    }
}

GLuint Shader::getId() const
{
    return m_id;
}

void Shader::setReloadOnChange(bool new_flag_value)
{
    m_reload_on_file_change = new_flag_value;
}

bool Shader::getReloadOnChange() const
{
    return m_reload_on_file_change;
}

VariablesData &Shader::getVariables()
{
    return m_variables;
}

const std::string &Shader::getFragmentPath()
{
    return m_fragment_path;
}
const std::string &Shader::getVertexPath()
{
    return m_vertex_path;
}

// ShaderUIData::ShaderUIData(Shader &program)
//     : p_program(&program), filename(program.getFragmentPath()), variables(program.getVariables())
// {
//     if (!filename.empty())
//     {
//         last_write_time = std::filesystem::last_write_time(filename);
//     }
// }

//! \brief some utility functions
//! \returns a vector of strings obtained from line
std::vector<std::string> separateLine(std::string line, char delimiter)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = 0;

    while ((start = line.find_first_not_of(delimiter, end)) != std::string::npos)
    {
        end = line.find(delimiter, start);
        result.push_back(line.substr(start, end - start));
    }
    return result;
}

//! \brief removes trailing spaces before and after the \p input
//! \param input
//! \returns the trimmed string
std::string trim(const std::string &input)
{

    auto last_nonspace = input.find_last_not_of(' ');
    auto first_nonspace = input.find_first_not_of(' ');
    if (last_nonspace == std::string::npos || first_nonspace == std::string::npos)
    {
        return "";
    }
    std::string result = input.substr(first_nonspace, last_nonspace);
    return result;
}

bool replace(std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

//! \brief extracts the value from the string obtained by reading a framgent shader
//! \param type_string  should contain part of the GLSL uniform definition with type and variable name
//! \param initial_value should contain part behind the equal sign containing the initial value
UniformType extractValue(std::string type_string, std::string initial_value)
{
    UniformType value;

    if (initial_value.empty()) //! when there is no string with initial values we set value to some default values
    {
        if (type_string == "float")
        {
            value = 0.f;
        }
        else if (type_string == "int")
        {
            value = 0;
        }
        else if (type_string == "vec3")
        {
            value = glm::vec3(1, 1, 1);
        }
        else if (type_string == "vec4")
        {
            value = glm::vec4(0, 0, 0, 0);
        }
    }
    else
    { //! we set value to
        auto value_string = trim(initial_value);
        if (type_string == "float")
        {
            value = std::stof(value_string);
        }
        else if (type_string == "int")
        {
            value = std::stoi(value_string);
        }
        else if (type_string == "vec2")
        {
            replace(value_string, type_string, "");
            replace(value_string, "(", "");
            replace(value_string, ")", "");
            auto values = separateLine(value_string, ',');
            value = glm::vec2(std::stof(values[0]), std::stof(values[1]));
        }
        else if (type_string == "vec3")
        {
            replace(value_string, type_string, "");
            replace(value_string, "(", "");
            replace(value_string, ")", "");
            auto values = separateLine(value_string, ',');
            value = glm::vec3(std::stof(values[0]), std::stof(values[1]), std::stof(values[2]));
        }
        else if (type_string == "vec4")
        {
            replace(value_string, type_string, "");
            replace(value_string, "(", "");
            replace(value_string, ")", "");
            auto values = separateLine(value_string, ',');
            value = glm::vec4(std::stof(values[0]), std::stof(values[1]), std::stof(values[2]), std::stof(values[3]));
        }
    }
    return value;
}

Shader2::Shader2(const std::string &source_code, ShaderType type)
    : m_type(type)
{
    m_id = glCreateShader(getGLCode(m_type));
    compile(source_code);
}
Shader2::Shader2(const std::filesystem::path &code_path, ShaderType type)
    : m_type(type)
{
    m_id = glCreateShader(getGLCode(m_type));
    // compile();
}

Shader2::~Shader2()
{
    glDeleteShader(m_id);
}

GLuint Shader2::getId() const
{
    return m_id;
}

bool Shader2::isCompiled() const
{
    return m_compiled;
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
#ifndef __EMSCRIPTEN__
    case st::Geometry:
        return "Geometry";
    case st::TessControl:
        return "Tesselation Control";
    case st::TessEvaluation:
        return "Tesselation Evaluation";
    case st::Compute:
        return "Compute";
#endif
    };
    return "";
}

bool Shader2::compile(const std::string &source_code)
{
    const char *shader_code = "";
    glShaderSource(m_id, 1, &shader_code, NULL);
    glCompileShader(m_id);
    glCheckError();

    // print compile errors if any
    GLint success;
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        int log_length;
        glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &log_length);
        std::string log_info_text;
        log_info_text.resize(log_length + 1);

        glGetShaderInfoLog(m_id, log_length, NULL, log_info_text.data());
        std::cout << "ERROR IN COMPILATION OF SHADER TYPE: " << getShaderTypeName(m_type) << "\n"
                  << log_info_text << std::endl;
        return false;
    };

    m_compiled = true;
    return true;
}

ShaderProgram::ShaderProgram()
{
    m_id = glCreateProgram();
}
ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_id);
}

void ShaderProgram::pushShader(const std::string &shader_code, ShaderType type)
{
    Shader2 shader(shader_code, type);

    extractUniformNamesFromCode(m_uniforms, shader_code);
    extractTextureNamesFromCode(m_uniforms, shader_code);

    if (!shader.isCompiled())
    {
        m_valid = false;
        return;
    }
    glAttachShader(m_id, shader.getId());
}

void ShaderProgram::linkShaders()
{
    if (m_valid)
    {
        glLinkProgram(m_id);
        glCheckError();
        m_linked = true;
    }
}

void ShaderProgram::use() 
{
    if (m_linked)
    {
        glUseProgram(m_id);
        updateUniforms();
    }
    else
    {
        std::cout << "WARNING: Trying to use an unlinked shader!" << std::endl;
    }
}

bool ShaderProgram::isValid() const
{
    return m_valid;
}

VariablesData &ShaderProgram::getUniforms()
{
    return m_uniforms;
}

void ShaderProgram::setUniform(const std::string &uniform_name, UniformType value)
{
    if (!m_uniforms.uniforms.contains(uniform_name))
    {
        std::cout << "WARNING: Uniform Name: " << uniform_name << " Does not exist!" << std::endl;
        return;
    }

    m_uniforms.uniforms.at(uniform_name) = {value, true};
}

//! \param name     name of the uniform
//! \param value    value of the uniform (the gl call is determined by the value type)
template <class ValueType>
void updateUniform(GLuint m_id, const std::string &name, const ValueType &value)
{
    if constexpr (std::is_same_v<ValueType, UniformType>)
    {
    }
    else if constexpr (std::is_same_v<ValueType, float>)
    {
        setFloat(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, bool>)
    {
        setBool(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, int>)
    {
        setInt(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::vec2>)
    {
        setVec2(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::vec3>)
    {
        setVec3(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::vec4>)
    {
        setVec4(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::mat2>)
    {
        setMat2(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::mat3>)
    {
        setMat3(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::mat4>)
    {
        setMat4(m_id, name, value);
    }
    else if constexpr (std::is_same_v<ValueType, Color>)
    {
        setVec4(m_id, name, glm::vec4(value.r, value.g, value.b, value.a));
    }
    else
    {
    }
}

void ShaderProgram::updateUniforms()
{

    for (auto &[key, uniform] : m_uniforms.uniforms)
    {
        if (uniform.needs_update)
        {
            auto update_value = [&key, this](auto &&v)
            {
                using T = std::decay_t<decltype(v)>;
                updateUniform<T>(m_id, key, v);
            };
            glCheckErrorMsg((key + " does not exist in the shader").c_str());
            std::visit(update_value, uniform.value);

            uniform.needs_update = false;
        }
    }

    //! This is retarded, I should just have start-time attribute as part of vertex attributes or something...
    if (m_uniforms.uniforms.contains("u_time"))
    {
        m_uniforms.uniforms.at("u_time") = {Shader::m_time, true};
        updateUniform(m_id, "u_time", Shader::m_time);
    }

    for (auto &[key, uniform_tex] : m_uniforms.textures)
    {
        if (uniform_tex.needs_update)
        {
            setUniform(key, uniform_tex.slot);
            uniform_tex.needs_update = false;
        }
    }
}