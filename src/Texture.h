#pragma once

#include "IncludesGl.h"
#include "Vertex.h"

#include <string>
#include <memory>
#include <map>

class Texture
{
    using GLuint = unsigned int;
    
    enum class ImageFormat
    {
        Jpg,
        Png,
    };

public:
    void loadFromFile(std::string filename);
    void create(int width, int height,  GLint internal_format = GL_RGBA, GLint format = GL_RGBA, GLint channel_format = GL_UNSIGNED_BYTE);
    void bind(int slot =0);
    Vec2 getSize()const;
    GLuint getHandle()const;
private:
    void invalidate();
    void initialize();
private:
    GLuint m_texture_handle = 0;

    int m_width = 0;
    int m_height = 0;
};



struct TextureHolder
{


    bool add(std::string texture_name, Texture& texture)
    {
        if(m_textures.count(texture_name) != 0)
        {
            std::cout <<"TEXTURE NAME EXISTS!\n" << texture_name << "\n";
            return false;
        }

        m_textures[texture_name] = std::make_shared<Texture>(texture);

        return true;
    }

    bool remove(std::string name)
    {

    }

    bool add(std::string texture_name, std::string filename)
    {
        if(m_textures.count(texture_name) != 0)
        {
            std::cout <<"TEXTURE NAME EXISTS!\n" << texture_name << "\n";
            return false;
        }
        auto tex = std::make_shared<Texture>();
        tex->loadFromFile(filename);
        m_textures[texture_name] = std::move(tex);
        return true;
    }

    std::shared_ptr<Texture> get(std::string name)
    {
        if(m_textures.count(name) > 0)
            return m_textures.at(name);

        return nullptr;
    }


    std::map<std::string, std::shared_ptr<Texture>> m_textures;

};
