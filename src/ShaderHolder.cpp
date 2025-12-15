#include "ShaderHolder.h"

#include "Shader.h"

//! \brief sets base path for searching shaders when loading
//! \param directory    path to a directory
//! \returns true if the \p directory is actually an existing directory, otherwise returns false
bool ShaderHolder::setBaseDirectory(std::filesystem::path directory)
{
    m_resources_path = directory;
    return true;
    // if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory))
    // {
    //     return true;
    // }

    // return false;
}

//! \brief default constructs with resource path being "../Resources/Shaders/"
ShaderHolder::ShaderHolder()
{
    m_resources_path = std::filesystem::path{"../Resources/Shaders/"};
}

//! \brief constructs with specified \p resources_path
//! \param resources_path   path to directory where you store your shaders
ShaderHolder::ShaderHolder(std::filesystem::path resources_path)
    : m_resources_path(resources_path)
{
}

//! \brief removes shader with \p shader_id from the holder
//! \param shader_id   id of the shader to be removed
void ShaderHolder::erase(const std::string &shader_id)
{
    m_shaders.erase(shader_id);
    // m_shader_data.erase(shader_id);
}

const ShaderHolder::ShaderMap &ShaderHolder::getShaders() const
{
    return m_shaders;
}

//! \param shader_id   id of the shader to be removed
//! \returns true if the \p shader_id is contained in the holder
bool ShaderHolder::contains(const std::string &shader_id) const
{
    return m_shaders.count(shader_id) > 0;
}

// ShaderHolder::ShaderUIDataMap &ShaderHolder::getAllData()
// {
//     return m_shader_data;
// }

Shader &ShaderHolder::get(const std::string &id)
{
    return *m_shaders.at(id);
}

//! \brief calls glUseProgram() if the \p shader_id is contained
//! \param shader_id   id of the shader to be removed
void ShaderHolder::use(const std::string &id)
{
    if (contains(id))
    {
        m_shaders.at(id)->use();
    }
}

//! \brief loads shader with id \p name
//! \brief vertex shader is located at: \p vertex_filename
//! \brief fragment shader is located at: \p fragment_filename
//! \brief the uniform names-values pairs are stored in \p shader_data
//! \param name
//! \param vertex_filename
//! \param fragment_filename
bool ShaderHolder::load(const std::string &name,
                        const std::string &vertex_filename, const std::string &fragment_filename)
{
    if (m_shaders.count(name) > 0) //! get rid of it first if shader with same name existed;
    {
        m_shaders.erase(name);
        // m_shader_data.erase(name);
    }

    std::filesystem::path vertex_path = m_resources_path.string() + vertex_filename; //! no idea if this works on windows?????
    std::filesystem::path fragment_path = m_resources_path.string() + fragment_filename;

    auto new_shader = std::make_unique<Shader>(vertex_path, fragment_path);
    if (!new_shader->wasSuccessfullyBuilt())
    {
        return false;
    }
    m_shaders[name] = std::move(new_shader);
    auto &shader = m_shaders.at(name);
    shader->m_shader_name = name;
    // m_shader_data.insert({name, *shader});

    shader->use();

    return true;
}

//! \brief loads shader with id \p name
//! \brief vertex shader code is directly in \p vertex_code string
//! \brief fragment shader code is directly in \p fragment_code string
//! \brief the uniform names-values pairs are stored in \p shader_data
//! \param name
//! \param vertex_code
//! \param fragment_code
//! \returns true if succesfully added
bool ShaderHolder::loadFromCode(const std::string &name,
                                const std::string &vertex_code, const std::string &fragment_code)
{
    if (m_shaders.count(name) > 0) //! get rid of it first if shader with same name existed;
    {
        return false; //! erasing fucks somethign up :(
        m_shaders.erase(name);
        // m_shader_data.erase(name);
    }

    auto new_shader = std::make_unique<Shader>(vertex_code, fragment_code);
    if (!new_shader->wasSuccessfullyBuilt())
    {
        return false;
    }
    //! if built add it to the holder
    m_shaders[name] = std::move(new_shader);
    auto &shader = m_shaders.at(name);
    shader->m_shader_name = name;
    // m_shader_data.insert({ name, *shader });
    return true;
}

//! \brief forces reload of all the shaders in the container
void ShaderHolder::refresh()
{
    std::vector<std::string> to_refresh;
    for (auto &[shader_name, shader] : m_shaders)
    {

        std::filesystem::path f_path = shader->getFragmentPath();
        auto last_time = std::filesystem::last_write_time(f_path);
        if (last_time != shader->m_last_writetime)
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