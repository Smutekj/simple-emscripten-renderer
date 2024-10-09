#include "Shader.h"




void static extractUniformNames(VariablesData &shader_data, const std::string &filename)
{
    const auto tmp_filename = filename + ".tmp";
    std::ifstream file(filename);
    if (!file.is_open())
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
            shader_data.uniforms[uniform_name] = value;
        }
    }

    file.close();
}

void static extractTextureNames(VariablesData &shader_data, std::string filename)
{
    const auto tmp_filename = filename + ".tmp";
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("File not found: " + filename);
    }

    GLuint texture_shader_id = 0;
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
    file.close();
}

std::string VariablesData::setTexture(int slot, GLuint handle)
{
    auto name_it = std::find_if(textures.begin(), textures.end(), [slot](auto &tex_data)
                                { return tex_data.second.slot == slot; });

    if (name_it != textures.end())
    {
        textures.at(name_it->first) = {slot, handle};
        return name_it->first;
    }
    return "";
}

Shader &ShaderHolder::get(std::string id)
{
    return *m_shaders.at(id);
}

void ShaderHolder::use(std::string id)
{
    m_shaders.at(id)->use();
}

void ShaderHolder::load(std::string name, std::string vertex_path, std::string fragment_path)
{
    if (m_shaders.count(name) > 0) //! get rid of it first if shader with same name existed;
    {
        m_shaders.erase(name);
        m_shader_data.erase(name);
    }
    m_shaders[name] = std::make_unique<Shader>(vertex_path, fragment_path);
    auto &shader = m_shaders.at(name);
    shader->m_shader_name = name;
    m_shader_data.insert({name, *shader});

    shader->use();
    extractUniformNames(m_shader_data.at(name).variables, shader->getFragmentPath());
}

ShaderUIData &ShaderHolder::getData(std::string name)
{
    return m_shader_data.at(name);
}

void ShaderHolder::initializeUniforms()
{
    for (auto &[shader_name, shader] : m_shaders)
    {
        m_shader_data.insert({shader_name, *shader});

        extractUniformNames(m_shader_data.at(shader_name).variables, shader->getFragmentPath());
    }
}
void ShaderHolder::refresh()
{
    std::vector<std::string> to_refresh;
    for (auto &[shader_name, shader] : m_shaders)
    {

        std::filesystem::path f_path = "/home/smutekj/Desktop/test/build/" + shader->getFragmentPath();
        auto last_time = std::filesystem::last_write_time(f_path);
        if (last_time != m_shader_data.at(shader_name).last_write_time)
        {
            to_refresh.push_back(shader_name);
        }
    }
    while (!to_refresh.empty())
    {
        auto shader_id = to_refresh.back();
        to_refresh.pop_back();
        auto vertex_path = m_shaders.at(shader_id)->getVertexPath();
        auto fragment_path = m_shaders.at(shader_id)->getFragmentPath();
    }
}

void Shader::setUniforms()
{
    for (auto &[name, value] : m_variables.uniforms)
    {
        setUniform2(name, value);
    }
    for (auto &[name, value] : m_variables.textures)
    {
        //! ???
    }
    if (m_variables.uniforms.contains("u_time"))
    {
        m_variables.uniforms.at("u_time") = Shader::m_time;
    }
    glCheckError();
}

template <class ValueType>
constexpr void Shader::setUniform(const std::string &name, const ValueType &value)
{

    if constexpr (std::is_same_v<ValueType, float>)
    {
        setFloat(name, value);
    }
    else if constexpr (std::is_same_v<ValueType, bool>)
    {
        setBool(name, value);
    }
    else if constexpr (std::is_same_v<ValueType, int>)
    {
        setInt(name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::vec2>)
    {
        setVec2(name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::vec3>)
    {
        setVec3(name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::vec4>)
    {
        setVec4(name, value);
    }
    else if constexpr (std::is_same_v<ValueType, glm::mat2>)
    {
        setMat2(name, value);
    }
    else if (std::is_same_v<ValueType, glm::mat3>)
    {
        setMat3(name, value);
    }
    else if (std::is_same_v<ValueType, glm::mat4>)
    {
        setMat4(name, value);
    }
    else
    {
    }
}

const std::string &Shader::getName() const
{
    return m_shader_name;
}

void Shader::activateTexture(std::array<GLuint, 2> handles)
{
    for (int slot = 0; slot < handles.size(); ++slot)
    {
        if (handles.at(slot) != 0)
        {
            auto name = m_variables.setTexture(slot, handles.at(slot));
            setInt(name, slot);
        }
    }
}

void Shader::setUniform2(std::string uniform_name, UniformType uniform_value)
{
    std::visit([&uniform_name, this](auto &&t)
               {
            using T = std::decay_t<decltype(t)>;
            setUniform<T>(uniform_name, t); }, uniform_value);
    glCheckErrorMsg((uniform_name + " does not exist in the shader").c_str());

    if (m_variables.uniforms.count(uniform_name) > 0)
    {
        m_variables.uniforms.at(uniform_name) = uniform_value;
    }
    else
    {
        m_variables.uniforms[uniform_name] = uniform_value;
    }
}

void Shader::saveUniformValue(std::string uniform_name, UniformType uniform_value)
{

    m_variables.uniforms.at(uniform_name) = uniform_value;
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
    extractTextureNames(m_variables, m_fragment_path);
    extractUniformNames(m_variables, m_fragment_path);

    glsl_include::ShaderLoader vertex_loader = glsl_include::ShaderLoader("#include");
    glsl_include::ShaderLoader fragment_loader = glsl_include::ShaderLoader("#include");
    std::string vertex_code = vertex_loader.load_shader(m_vertex_path);
    std::string fragment_code = vertex_loader.load_shader(m_fragment_path);

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
    auto last_time = std::filesystem::last_write_time(m_fragment_path);
    if (last_time != m_last_writetime)
    {
        if (m_id != -1)
        {
            glDeleteProgram(m_id);
        }
        recompile();
        m_last_writetime = last_time;
    }
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

ShaderUIData::ShaderUIData(Shader &program)
    : p_program(&program), filename(program.getFragmentPath()), variables(program.getVariables())
{
    last_write_time = std::filesystem::last_write_time(filename);
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

void ShaderHolder::erase(const std::string &shader_id)
{
    m_shaders.erase(shader_id);
    m_shader_data.erase(shader_id);
}

const std::unordered_map<std::string, std::unique_ptr<Shader>> &ShaderHolder::getShaders() const
{
    return m_shaders;
}

bool ShaderHolder::contains(const std::string &shader_id) const
{
    return m_shaders.count(shader_id) > 0;
}

auto &ShaderHolder::getAllData()
{
    return m_shader_data;
}


std::vector<std::string> separateLine(std::string line, char delimiter)
{
    std::vector<std::string> result;
    int start = 0;
    int end = 0;

    while ((start = line.find_first_not_of(delimiter, end)) != std::string::npos)
    {
        end = line.find(delimiter, start);
        result.push_back(line.substr(start, end - start));
    }
    return result;
}

std::string trim(std::string input)
{
    std::string result;
    for (auto c : input)
    {
        if (c != ' ' && c != ';')
        {
            result.push_back(c);
        }
    }
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
