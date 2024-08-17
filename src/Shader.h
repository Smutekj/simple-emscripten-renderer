#pragma once

#include "IncludesGl.h"
#include "ShaderLoader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <type_traits>
#include <variant>

#include <magic_enum.hpp>
#include <magic_enum_utility.hpp>

// helper type for the visitor (stolen from cpp_reference)
template <class... Ts>
struct overloads : Ts...
{
    using Ts::operator()...;
};

class ShaderHolder;

struct TextureGlData
{
    int slot = 0;
    GLuint handle = 0;
};

using UniformType = std::variant<float, bool, int, glm::vec4, glm::vec3, glm::vec2>;

struct VariablesData
{
    std::unordered_map<std::string, UniformType> uniforms;
    std::unordered_map<std::string, TextureGlData> textures;
};

class Shader
{

public:
    Shader() = default;
    Shader(const std::string vertex_path, const std::string fragment_path);
    Shader(const std::string vertex_path, const std::string fragment_path, std::string shader_name);

    VariablesData &getVariables()
    {
        return m_variables;
    }

    GLuint getId() const;
    void recompile();
    const std::string &getFragmentPath()
    {
        return m_fragment_path;
    }
    const std::string &getVertexPath()
    {
        return m_vertex_path;
    }
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

    void setUniforms()
    {
        for (auto &[name, value] : m_variables.uniforms)
        {
            setUniform2(name, value);
        }
        for (auto &[name, value] : m_variables.textures)
        {
        }
        glCheckError();
    }

    template <class ValueType>
    constexpr void setUniform(const std::string &name, const ValueType &value)
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

    const std::string &getName() const
    {
        return m_shader_name;
    }

    void activateTexture(int slot)
    {
        for (auto &[name, handle] : m_variables.textures)
        {
            setInt(name, handle.slot);
        }
    }

public:
    void setUniform2(std::string uniform_name, UniformType uniform_value)
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

    void saveUniformValue(std::string uniform_name, UniformType uniform_value)
    {

        m_variables.uniforms.at(uniform_name) = uniform_value;
    }

    friend ShaderHolder;

private:
    void retrieveCode(const char *code_path, std::string &code);

    unsigned int m_id; // the program ID
    std::string m_fragment_path;
    std::string m_vertex_path;
    std::string m_shader_name = "default_name";

    VariablesData m_variables;
};

void inline bindVertexAttributes(GLuint buffer, std::vector<int> sizes)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glCheckError();
    int offset = 0;
    auto total_size = std::accumulate(sizes.begin(), sizes.end(), 0);
    for (std::size_t i = 0; i < sizes.size(); ++i)
    {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, sizes.at(i), GL_FLOAT, GL_FALSE,
                              total_size * sizeof(float), (void *)(offset * sizeof(float)));
        glVertexAttribDivisor(i, 0);
        glCheckError();
        offset += sizes.at(i);
    }
}

class ShaderS
{

public:
    ShaderS() = default;
    ShaderS(const char *vertexPath, const char *fragmentPath);

    GLuint getId() const;
    void recompile();

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

private:
    void retrieveCode(const char *code_path, std::string &code);

    unsigned int m_id; // the program ID
    std::string m_fragment_path;
    std::string m_vertex_path;
    std::string m_shader_name = "default_name";
};

enum class ShaderID
{
    VertexArrayDefault,
    ShapeDefault,
    Texture,
    Instanced,
    InstancedBorder,
    Test,
    TestChaining,
    Line,
};

struct ShaderUIData
{

    ShaderUIData(Shader &program)
        : p_program(&program), filename(program.getFragmentPath()), variables(program.getVariables())
    {
    }

    Shader *p_program = nullptr;
    std::string filename = "";
    VariablesData &variables;
};

void inline extractUniformNames(VariablesData &shader_data, const std::string &filename);

class ShaderHolder
{

public:
    Shader &get(std::string id)
    {
        return *m_shaders.at(id);
    }

    void use(std::string id)
    {
        m_shaders.at(id)->use();
    }

    void load(std::string name, std::string vertex_path, std::string fragment_path)
    {

        m_shaders[name] = std::make_unique<Shader>(vertex_path, fragment_path);
        auto &shader = m_shaders.at(name);
        shader->m_shader_name = name;
        m_shader_data.insert({name, *shader});
        shader->use();
        extractUniformNames(m_shader_data.at(name).variables, shader->getFragmentPath());
    }

    const auto &getShaders() const
    {
        return m_shaders;
    }

    ShaderUIData &getData(std::string name)
    {
        return m_shader_data.at(name);
    }
    auto &getAllData()
    {
        return m_shader_data;
    }

    void initializeUniforms()
    {
        for (auto &[shader_name, shader] : m_shaders)
        {
            m_shader_data.insert({shader_name, *shader});

            extractUniformNames(m_shader_data.at(shader_name).variables, shader->getFragmentPath());
        }
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;
    std::unordered_map<std::string, ShaderUIData> m_shader_data;
};

std::vector<std::string> inline separateLine(std::string line, char delimiter = ' ')
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

inline std::string trim(std::string input)
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

inline bool replace(std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

inline UniformType extractValue(std::string type_string, std::string initial_value)
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

void inline extractUniformNames(VariablesData &shader_data, const std::string &filename)
{
    const auto tmp_filename = filename + ".tmp";
    std::ifstream file(filename);

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

void inline extractTextureNames(VariablesData &shader_data, std::string filename)
{
    const auto tmp_filename = filename + ".tmp";
    std::ifstream file(filename);

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
            if (texture_var_name.back() == ';')
            {
                texture_var_name.pop_back();
            }
            // shader_data.p_program->setUniform2(texture_var_name, texture_shader_id);
            shader_data.textures[texture_var_name] = {slot, 0};
            slot++;
        }
    }
    file.close();
}