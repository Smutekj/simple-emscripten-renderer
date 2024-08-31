#include "Font.h"
#include "IncludesGl.h"

#include <Renderer.h>

void Font::initialize()
{
}

Font::Font(std::string font_filename)
{
    TextureOptions options;
    options.data_type = TextureDataTypes::UByte;
    options.format = TextureFormat::RGBA;
    options.internal_format = TextureFormat::RGBA;
    m_pixels = std::make_unique<FrameBuffer>(400, 400, options);
    m_canvas = std::make_unique<Renderer>(*m_pixels);

    using Path = std::filesystem::path;
    Path shaders_path(__FILE__);
    shaders_path.remove_filename();
    shaders_path += "../Resources/";
    Path vertex_path = shaders_path.string() + "basicinstanced.vert";
    Path fragment_path = shaders_path.string() + "text.frag";
    m_canvas->addShader("Text", vertex_path, fragment_path);
    m_pixels->clear({0, 0, 0, 1});
    if (!loadFromFile(font_filename))
    {
        // spdlog::error("FONT FILE " + font_filename + " NOT FOUND!");
    }
}

void inline printBuffer(const FT_Face &face)
{
    auto w = face->glyph->bitmap.width;
    auto h = face->glyph->bitmap.rows;
    for (auto iy = 0; iy < h; ++iy)
    {
        std::string text = "";
        for (auto ix = 0; ix < w; ++ix)
        {
            int ind = iy * w + ix;
            bool wtf = face->glyph->bitmap.buffer[ind];
            text += std::to_string(wtf);
        }
        text += "\n";
        std::cout << text;
    }
}

bool Font::loadFromFile(std::string font_filename)
{
    std::string font_fullpath = "../Resources/Fonts/" + font_filename;

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        // spdlog::error("FREETYPE: Could not init FreeType Library");
        return false;
    }

    FT_Face face;
    if (FT_New_Face(ft, font_fullpath.c_str(), 0, &face))
    {
        // spdlog::error("FREETYPE: Failed to load font");
        return false;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    glCheckError();

    unsigned int max_width = 0;
    unsigned int max_height = 0;
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        max_width = std::max(face->glyph->bitmap.width, max_width);
        max_height = std::max(face->glyph->bitmap.rows, max_height);
    }

    // GLuint texture_ids[128];
    // glGenTextures(128, texture_ids);

    auto &main_texture = m_pixels->getTexture();
    m_canvas->m_view.setCenter(main_texture.getSize() / 2.f);
    m_canvas->m_view.setSize(main_texture.getSize());
    m_canvas->clear({1, 1, 1, 0});

    utils::Vector2i glyph_pos = {0, 0};
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character =
            {
                main_texture.getHandle(),
                glyph_pos,
                {face->glyph->bitmap.width, face->glyph->bitmap.rows},
                {face->glyph->bitmap_left, face->glyph->bitmap_top},
                face->glyph->advance.x};
        m_characters.insert(std::pair<char, Character>(c, character));

        Sprite2 glyph_sprite(main_texture);
        glyph_sprite.m_tex_rect = {0, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows};
        glyph_sprite.m_texture_handles[0] = texture;
        glyph_sprite.m_tex_size = character.size;
        glyph_sprite.setPosition(glyph_pos + (character.size + 1) / 2.f);
        glyph_sprite.setScale(face->glyph->bitmap.width / 2.f,
                              -static_cast<float>(face->glyph->bitmap.rows) / 2.f); //! MINUS FOR Y COORD IS IMPORTANT!!!!!!
        m_canvas->drawSprite(glyph_sprite, "Text", GL_DYNAMIC_DRAW);

        glyph_pos.x += face->glyph->bitmap.width;
        if (glyph_pos.x + max_width >= main_texture.getSize().x) //! if we reach right side of the main texture
        {
            glyph_pos.y += max_height;
            glyph_pos.x = 0;
        }
        //! delete helper texture
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_canvas->drawAll();
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // glDeleteTextures(128, texture_ids);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); //! set back to deafult value

    return true;
}

Texture &Font::getTexture()
{
    return m_pixels->getTexture();
}