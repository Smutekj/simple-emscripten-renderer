#pragma once

#include <unordered_map>
#include <filesystem>

class Shader;


//! \class ShaderHolder
//! \brief holds shaders themselves and also data about uniforms and textures in them
class ShaderHolder
{
    using ShaderMap = std::unordered_map<std::string, std::shared_ptr<Shader>>;

public:
    ShaderHolder();
    explicit ShaderHolder(std::filesystem::path resources_path);

    Shader &get(const std::string &id);

    void use(const std::string &id);

    bool load(const std::string &name, const std::string &vertex_filename, const std::string &fragment_filename);
    bool loadFromCode(const std::string &id, const std::string &vertex_code, const std::string &fragment_code);

    void erase(const std::string &shader_id);

    const ShaderMap &getShaders() const;

    bool contains(const std::string &shader_id) const;

    void refresh();

    bool setBaseDirectory(std::filesystem::path dir);

private:
    ShaderMap m_shaders;
    std::filesystem::path m_resources_path;
};