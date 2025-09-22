#include "Font.h"
#include "IncludesGl.h"
#include "CommonShaders.inl"

#include "Renderer.h"
#include "FrameBuffer.h"

//! \brief creates a font from a path to a file
//! \param font_filename path to a font file
Font::Font(std::filesystem::path font_filename)
{
    TextureOptions options;
    options.data_type = TextureDataTypes::UByte;
    options.format = TextureFormat::RGBA;
    options.internal_format = TextureFormat::RGBA;
    options.wrap_x = TexWrapParam::ClampEdge;
    options.wrap_y = TexWrapParam::ClampEdge;
    m_pixels = std::make_unique<FrameBuffer>(1000, 1000, options);
    m_canvas = std::make_unique<Renderer>(*m_pixels);
    m_canvas->getShaders().loadFromCode("Font",
                                        vertex_sprite_code,
                                        fragment_font_code);

    if (!loadFromFile(font_filename))
    {
        throw std::runtime_error("FONT FILE " + font_filename.string() + " NOT FOUND!");
    }
}

//! \brief just for debugging
[[maybe_unused]] void static printBuffer(const FT_Face &face)
{
    auto w = face->glyph->bitmap.width;
    auto h = face->glyph->bitmap.rows;
    for (unsigned int iy = 0; iy < h; ++iy)
    {
        std::string text = "";
        for (unsigned int ix = 0; ix < w; ++ix)
        {
            int ind = iy * w + ix;
            bool wtf = face->glyph->bitmap.buffer[ind];
            text += std::to_string(wtf);
        }
        text += "\n";
        std::cout << text;
    }
}

//! \brief loads font from the specified file
//! \return true if font was succesfully loaded
bool Font::loadFromFile(std::filesystem::path font_file)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        // spdlog::error("FREETYPE: Could not init FreeType Library");
        return false;
    }

    FT_Face face;
#if defined(__ANDROID__)
    SDL_RWops *rw = SDL_RWFromFile(font_file.c_str(), "rb");
    // On Android this reads from assets.

    if (!rw)
    {
        SDL_Log("Could not open font: %s", SDL_GetError());
    }

    // Read file into memory
    Sint64 size = SDL_RWsize(rw);
    std::vector<unsigned char> buffer(size);
    SDL_RWread(rw, buffer.data(), 1, size);
    SDL_RWclose(rw);
    if (FT_New_Memory_Face(ft, buffer.data(), buffer.size(), 0, &face))
    {
        return false;
    }
#else
    if (FT_New_Face(ft, font_file.string().c_str(), 0, &face))
    {
        return false;
    }
#endif

    FT_Set_Pixel_Sizes(face, 0, 40);

    FT_GlyphSlot slot = face->glyph;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    glCheckError();

    unsigned int max_width = 0;
    unsigned int max_height = 0;

    FT_ULong charcode;
    FT_UInt gindex;

    int char_count = 0;
    charcode = FT_Get_First_Char(face, &gindex);
    while (gindex != 0)
    {
        FT_Load_Char(face, charcode, FT_LOAD_BITMAP_METRICS_ONLY);

        max_width = std::max(face->glyph->bitmap.width, max_width);
        max_height = std::max(face->glyph->bitmap.rows, max_height);

        charcode = FT_Get_Next_Char(face, charcode, &gindex);
        char_count++;
    }

    unsigned int atlas_texture;
    // glGenTextures(char_count, textures);
    glGenTextures(1, &atlas_texture);
    glCheckError();

    glBindTexture(GL_TEXTURE_2D, atlas_texture);
    glCheckError();
    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_R8,
        1000,
        1000,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        nullptr);
    glCheckError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glCheckError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glCheckError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glCheckError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glCheckError();

    int safety_pixels_x = 3;
    int safety_pixels_y = 10;

    auto &main_texture = m_pixels->getTexture();
    m_canvas->m_view.setCenter(main_texture.getSize() / 2.f);
    m_canvas->m_view.setSize(main_texture.getSize().x, -main_texture.getSize().y);
    m_canvas->clear({1, 1, 1, 0});
    m_canvas->m_blend_factors = {BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha};

    utils::Vector2i glyph_pos = {0, 0};

    charcode = FT_Get_First_Char(face, &gindex);
    int i = 0;
    while (gindex != 0)
    {
        // load character glyph
        if (FT_Load_Char(face, charcode, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);

        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            glyph_pos.x,
            1000-glyph_pos.y - face->glyph->bitmap.rows,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer);

        Character character =
            {
                main_texture.getHandle(),
                glyph_pos,
                {face->glyph->bitmap.width, face->glyph->bitmap.rows},
                {face->glyph->bitmap_left, face->glyph->bitmap_top},
                (unsigned int)face->glyph->advance.x};
        m_characters.insert(std::pair<int, Character>(charcode, character));

        Sprite glyph_sprite(main_texture);
        glyph_sprite.m_tex_rect = {glyph_pos.x, glyph_pos.y,
                                   (int)face->glyph->bitmap.width, (int)face->glyph->bitmap.rows};
        glyph_sprite.m_texture_handles[0] = atlas_texture;
        glyph_sprite.m_tex_size = m_pixels->getSize();

        glyph_sprite.setPosition(glyph_pos + (character.size + 1) / 2.f);
        glyph_sprite.setScale(face->glyph->bitmap.width / 2.f,
                              static_cast<float>(face->glyph->bitmap.rows) / 2.f); //! MINUS FOR Y COORD IS IMPORTANT!!!!!!
        m_canvas->drawSprite(glyph_sprite, "Font", DrawType::Dynamic);

        glyph_pos.x += face->glyph->bitmap.width + safety_pixels_x;
        if (glyph_pos.x + max_width >= main_texture.getSize().x) //! if we reach right side of the main texture
        {
            glyph_pos.y += max_height + safety_pixels_y;
            glyph_pos.x = 0;
        }

        charcode = FT_Get_Next_Char(face, charcode, &gindex);
        i++;
    }
    m_canvas->drawAll();
    m_canvas->resetBatches();
    // writeTextureToFile("../", "pica.png", *m_pixels);

    //! delete helper texture
    glDeleteTextures(1, &atlas_texture);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); //! set back to deafult value

    return true;
}

//! \return a texture containing character SDFs
Texture &Font::getTexture()
{
    return m_pixels->getTexture();
}
