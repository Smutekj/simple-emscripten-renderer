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

#include <magic_enum.hpp>
#include <magic_enum_utility.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/type_ptr.hpp>

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

    std::string setTexture(int slot, GLuint handle)
    {
        auto name = std::find_if(textures.begin(), textures.end(), [slot](auto &tex_data)
                                 { return tex_data.second.slot == slot; })
                        ->first;

        textures.at(name) = {slot, handle};
        return name;
    }
};

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

    unsigned int m_id; // the program ID
    std::string m_fragment_path;
    std::string m_vertex_path;
    std::string m_shader_name = "default_name";

    VariablesData m_variables;

public:
    inline static float m_time;
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

class ShaderHolder
{

public:
    Shader &get(std::string id);

    void use(std::string id);

    void load(std::string name, std::string vertex_path, std::string fragment_path);

    const auto &getShaders() const
    {
        return m_shaders;
    }

    bool contains(const std::string &shader_id) const
    {
        return m_shaders.count(shader_id) > 0;
    }

    ShaderUIData &getData(std::string name);
    auto &getAllData()
    {
        return m_shader_data;
    }

    void initializeUniforms();

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
