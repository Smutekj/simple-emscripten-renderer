#pragma once

#include "Texture.h"

#include <string>
#include <filesystem>
#include <unordered_map>

//! \class TextureHolder
//! \brief holds textures based on id given by string
class TextureHolder
{

public:
    bool add(const std::string& texture_name, Texture &texture);
    bool add(const std::string& texture_name, const std::string& filename, TextureOptions opt = {});
    bool add(const std::string& texture_name, const std::filesystem::path& texture_file_path, TextureOptions opt = {});
    bool add(const std::string& texture_name, const unsigned char *buffer, std::size_t size, TextureOptions opt = {});

    void erase(const std::string &texture_id);

    std::shared_ptr<Texture> get(const std::string& name) const;
    std::unordered_map<std::string, std::shared_ptr<Texture>> &getTextures();

    bool setBaseDirectory(const std::filesystem::path& directory);

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    std::filesystem::path m_resources_path;
};