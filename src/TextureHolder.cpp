#include "TextureHolder.h"

//! \brief adds texture into the holder under id \p texture_name
//! \param texture_name our id of the texture
//! \param texture
//! \returns true if no texture of this name exists othrewise return false;
bool TextureHolder::add(const std::string &texture_name, Texture &texture)
{
    if (m_textures.count(texture_name) != 0)
    {
        return false;
    }

    m_textures[texture_name] = std::make_shared<Texture>(texture);

    return true;
}

bool TextureHolder::add(const std::string &texture_name, const std::filesystem::path &texture_file_path, TextureOptions opt)
{
    if (m_textures.count(texture_name) != 0)
    {
        return false;
    }

    auto tex = std::make_shared<Texture>();
    tex->loadFromFile(texture_file_path.string(), opt);
    m_textures[texture_name] = std::move(tex);
    return true;
}
//! \brief reads the texture in \p texture_filename and adds it
//! \brief into the holder under id \p texture_name
//! \param texture_name our id of the texture
//! \param texture_filename     filename of the texture
//! \returns true if no texture of this name exists othrewise return false;
bool TextureHolder::add(const std::string &texture_name, const std::string &texture_filename, TextureOptions opt)
{
    return add(texture_name, m_resources_path / texture_filename, opt);
}

bool TextureHolder::add(const std::string &texture_name, const unsigned char *buffer, std::size_t size, TextureOptions opt)
{
    if (m_textures.count(texture_name) != 0)
    {
        return false;
    }

    auto tex = std::make_shared<Texture>();
    tex->loadFromBytes(buffer, size, opt);
    m_textures[texture_name] = std::move(tex);
    return true;
}

std::shared_ptr<Texture> TextureHolder::get(const std::string &name) const
{
    if (m_textures.count(name) > 0)
    {
        return m_textures.at(name);
    }

    return nullptr;
}

std::unordered_map<std::string, std::shared_ptr<Texture>> &TextureHolder::getTextures()
{
    return m_textures;
}

//! \brief sets base path for searching shaders when loading
//! \param directory    path to a directory
//! \returns true if the \p directory is actually an existing directory, otherwise returns false
bool TextureHolder::setBaseDirectory(const std::filesystem::path& directory)
{
    // if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
    // {
    //     return false;
    // }

    m_resources_path = directory;
    return true;
}

const TextureOptions &Texture::getOptions() const
{
    return m_options;
}

void TextureHolder::erase(const std::string &texture_id)
{
    m_textures.erase(texture_id);
}
