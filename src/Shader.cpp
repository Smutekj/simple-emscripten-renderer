#include "Shader.h"

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
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
}

Shader::Shader(const std::string vertex_path, const std::string fragment_path,
               std::string shader_name) : m_shader_name(shader_name),
                                          m_vertex_path(vertex_path),
                                          m_fragment_path(fragment_path)
{
    recompile();
}

Shader::Shader(const std::string vertex_path, const std::string fragment_path)
    : m_vertex_path(vertex_path),
      m_fragment_path(fragment_path)
{

    recompile();
}

void Shader::recompile()
{
    // glDeleteProgram(m_id);

    extractTextureNames(m_variables, m_fragment_path);
    extractUniformNames(m_variables, m_fragment_path);

    glsl_include::ShaderLoader vertex_loader = glsl_include::ShaderLoader("#include");
    glsl_include::ShaderLoader fragment_loader = glsl_include::ShaderLoader("#include");
    std::string vertex_code = vertex_loader.load_shader(m_vertex_path);
    std::string fragment_code = vertex_loader.load_shader(m_fragment_path);

    // retrieveCode(m_vertex_path.c_str(), vertex_code);
    // retrieveCode(m_fragment_path.c_str(), fragment_code);
    const char *vShaderCode = vertex_code.c_str();
    const char *fShaderCode = fragment_code.c_str();

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
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glCheckError();
}


void Shader::use()
{
    glUseProgram(m_id);
    glCheckError();
}

// utility uniform functions
void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
};
void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
};

GLuint Shader::getId() const
{
    return m_id;
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
    glCheckError();
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


// template <class ValueType>
// constexpr void Shader::setUniform(const std::string &name, const ValueType &value)
// {

//     if constexpr (std::is_same_v<ValueType, float>)
//     {
//         setFloat(name, value);
//     }
//     else if (std::is_same_v<ValueType, bool>)
//     {
//         setBool(name, value);
//     }
//     else if (std::is_same_v<ValueType, int>)
//     {
//         setInt(name, value);
//     }
//     else if (std::is_same_v<ValueType, glm::vec2>)
//     {
//         setVec2(name, value);
//     }
//     else if (std::is_same_v<ValueType, glm::vec3>)
//     {
//         setVec3(name, value);
//     }
//     else if (std::is_same_v<ValueType, glm::vec4>)
//     {
//         setVec4(name, value);
//     }
//     else if (std::is_same_v<ValueType, glm::mat2>)
//     {
//         setMat2(name, value);
//     }
//     else if (std::is_same_v<ValueType, glm::mat3>)
//     {
//         setMat3(name, value);
//     }
//     else if (std::is_same_v<ValueType, glm::mat4>)
//     {
//         setMat4(name, value);
//     }else
//     {

//     }
// }
