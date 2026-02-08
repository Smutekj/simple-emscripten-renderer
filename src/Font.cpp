#include "Font.h"
#include "IncludesGl.h"
#include "CommonShaders.inl"

#include "Renderer.h"
#include "FrameBuffer.h"
#include "Sprite.h"

#include <fstream>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H   //optional glyph management component (I keep these here because I'll probably need them )
#include FT_OUTLINE_H //scalable outline management
// #include FT_STROKER_H //functions to stroke outline paths

Font::~Font()
{
    FT_Done_Face(*mp_face);
    FT_Done_FreeType(*mp_ft);

    delete mp_ft;
    delete mp_face;
}

//! \brief creates a font from a path to a file
//! \param font_filename path to a font file
Font::Font(std::filesystem::path font_filename, size_t font_pixel_size, FreetypeMode mode)
    : m_mode(mode), m_font_pixel_size(font_pixel_size)
{
    mp_face = new FT_Face();
    mp_ft = new FT_Library();

    if (!loadFromFile(font_filename))
    {
        throw std::runtime_error("FONT FILE " + font_filename.string() + " NOT FOUND!");
    }
}
Font::Font(const unsigned char *bytes, std::size_t num_bytes, size_t font_pixel_size, FreetypeMode mode)
    : m_mode(mode), m_font_pixel_size(font_pixel_size)
{
    mp_face = new FT_Face();
    mp_ft = new FT_Library();

    if (!loadFromBytes(bytes, num_bytes))
    {
        throw std::runtime_error("UNABLE TO LOAD FONT");
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

float Font::getLineHeight() const
{
    return m_line_height;
}

bool Font::initializeFromFace(FT_Face &face)
{

    //! create texture to draw into
    TextureOptions options;
    options.data_type = TextureDataTypes::UByte;
    options.format = TextureFormat::RGBA;
    options.internal_format = TextureFormat::RGBA;
    options.wrap_x = TexWrapParam::ClampEdge;
    options.wrap_y = TexWrapParam::ClampEdge;
    options.mag_param = TexMappingParam::Linear;
    options.min_param = TexMappingParam::Linear;

    FT_Set_Pixel_Sizes(face, 0, m_font_pixel_size);
    m_line_height = face->size->metrics.height / 64.f;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    glCheckError();

    std::size_t font_texture_width = 2048; //! how to set this?
    std::size_t safety_margin = 2;         //! number of pixels that separate glyphs in texture
    std::size_t font_texture_height = 0;
    utils::Vector2<unsigned int> max_char_size = {0};

    //! find how large the font texture needs to be
    std::size_t line_pos_x = 0;
    int char_count = 0;
    FT_UInt gindex;
    FT_ULong charcode = FT_Get_First_Char(face, &gindex);
    while (gindex != 0)
    {
        FT_Load_Char(face, charcode, FT_LOAD_RENDER);

        FT_Render_Glyph(face->glyph, static_cast<FT_Render_Mode>(m_mode));
        unsigned int char_width = face->glyph->bitmap.width;
        unsigned int char_height = face->glyph->bitmap.rows;

        max_char_size = {std::max(char_width, max_char_size.x), std::max(char_height, max_char_size.y)};

        if (line_pos_x + char_width + safety_margin >= font_texture_width)
        {
            line_pos_x = 0;
            font_texture_height += max_char_size.y + safety_margin;
            max_char_size.y = 0;
        }
        line_pos_x += char_width + safety_margin;

        charcode = FT_Get_Next_Char(face, charcode, &gindex);
        char_count++;
    }
    font_texture_height += max_char_size.y;
    max_char_size.y = {0};

    m_pixels = std::make_unique<FrameBuffer>(font_texture_width, font_texture_height, options);
    m_canvas = std::make_unique<Renderer>(*m_pixels);
    m_canvas->getShaders().loadFromCode("Font",
                                        vertex_sprite_code,
                                        fragment_font_code);

    std::size_t atlas_w = m_pixels->getSize().x;
    std::size_t atlas_h = m_pixels->getSize().y;
    std::vector<uint8_t> atlas_pixels(atlas_w * atlas_h);

    TextureOptions helper_texture_options = {
        .format = TextureFormat::Red,
        .internal_format = TextureFormat::R8,
        .data_type = TextureDataTypes::UByte,
        .mag_param = TexMappingParam::Linear,
        .min_param = TexMappingParam::Linear,
        .mipmap_levels = 0};

    Texture atlas_texture;
    atlas_texture.create(atlas_w, atlas_h, helper_texture_options);
    atlas_texture.bind();

    auto &main_texture = m_pixels->getTexture();
    m_canvas->m_view.setCenter(main_texture.getSize() / 2.f);
    m_canvas->m_view.setSize(main_texture.getSize().x, -main_texture.getSize().y);
    m_canvas->clear({1, 1, 1, 0});
    m_canvas->m_blend_factors = {BlendFactor::One, BlendFactor::One};

    //! initialize characters data
    m_characters.clear();

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

        FT_GlyphSlot &glyph = face->glyph;
        FT_Bitmap &bitmap = glyph->bitmap;
        FT_Render_Glyph(glyph, static_cast<FT_Render_Mode>(m_mode));

        if (glyph_pos.x + bitmap.width + safety_margin >= main_texture.getSize().x) //! if we reach right side of the main texture
        {
            glyph_pos.y += max_char_size.y + safety_margin;
            glyph_pos.x = 0;
            max_char_size = {0};
        }
        max_char_size = {std::max(bitmap.width, max_char_size.x), std::max(bitmap.rows, max_char_size.y)};

        glTexSubImage2D(GL_TEXTURE_2D, 0,
                        glyph_pos.x, glyph_pos.y,
                        bitmap.width, bitmap.rows,
                        GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);
        glCheckError(); //! the error here is most likely due to rendering outside of texture

        FT_BBox bbox;
        FT_Outline_Get_CBox(&glyph->outline, &bbox);

        float width = (bbox.xMax / 64.f - bbox.xMin / 64.f);
        float height = (bbox.yMax / 64.f - bbox.yMin / 64.f);
        Rectf char_bb = {bbox.xMin / 64.f, bbox.yMin / 64.f, width, height};

        // for (int y = 0; y < bitmap.rows; ++y)
        // {
        //     int dest_y = glyph_pos.y + y;
        //     int dest_x = glyph_pos.x;
        //     memcpy(&atlas_pixels[dest_y * atlas_w + dest_x],
        //            bitmap.buffer + y * bitmap.pitch,
        //            bitmap.width);
        // }

        Character character =
            {
                main_texture.getHandle(),
                glyph_pos,
                {bitmap.width, bitmap.rows},
                {glyph->bitmap_left, glyph->bitmap_top},
                char_bb,
                (unsigned int)glyph->advance.x};
        m_characters.insert(std::pair<int, Character>(charcode, character));

        glyph_pos.x += bitmap.width + safety_margin; //! move position to next glyph

        charcode = FT_Get_Next_Char(face, charcode, &gindex);
        i++;
    }
    // GLuint tex;
    // glGenTextures(1, &tex);
    // glBindTexture(GL_TEXTURE_2D, tex);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, atlas_w, atlas_h, 0, GL_RED, GL_UNSIGNED_BYTE, atlas_pixels.data());

    Sprite glyph_sprite(main_texture);
    glyph_sprite.m_texture_handles[0] = atlas_texture.getHandle();
    glyph_sprite.setPosition(main_texture.getSize() / 2.f);
    glyph_sprite.setScale(main_texture.getSize() / 2.f);
    glCheckError();
    m_canvas->drawSprite(glyph_sprite, "Font");

    using bf = BlendFactor;
    m_canvas->m_blend_factors = {bf::One, bf::Zero, bf::One, bf::Zero};
    m_canvas->drawAll();
    m_canvas->resetBatches();
    // writeTextureToFile("../", "pica.png", *m_pixels);
    //! delete helper texture
    // glDeleteTextures(1, &tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); //! set back to deafult value

    TextureOptions text_options;
    text_options.data_type = TextureDataTypes::UByte;
    text_options.format = TextureFormat::RGBA;
    text_options.internal_format = TextureFormat::RGBA;
    text_options.mag_param = TexMappingParam::Linear;
    text_options.min_param = TexMappingParam::Linear;
    m_prerendered = std::make_unique<FrameBuffer>(atlas_w, atlas_h, text_options);
    Renderer canvas(*m_prerendered);
    glyph_pos = {0.f, m_font_pixel_size};
    float max_image_height = 0;
    canvas.clear({0, 0, 0, 0});
    canvas.m_view = canvas.getDefaultView();
    Sprite glyph(getTexture());
    glyph.setColor({255, 255, 255, 255});
    for (auto &[code, character] : m_characters)
    {
        float glyph_width = character.size.x * 2;
        float glyph_height = character.size.y * 2;
        float dy = character.size.y - character.bearing.y;

        utils::Vector2f pos = glyph_pos + Vec2{character.bearing.x + glyph_width / 2.f, glyph_height / 2.f - dy};
        glyph.m_tex_rect = {character.tex_coords.x, character.tex_coords.y,
                            character.size.x, character.size.y};

        glyph.setPosition(pos);
        glyph.setScale(glyph_width / 2., glyph_height / 2.);
        auto image_height = character.bb.height * 2;
        auto image_width = character.bb.width * 2;
        max_image_height = std::max(image_height, max_image_height);
        canvas.drawSprite(glyph, "TextDefault");

        glyph_pos.x += (character.advance >> 6) * 2;
        if (glyph_pos.x + image_width >= atlas_w)
        {
            glyph_pos.x = 0;
            glyph_pos.y += max_image_height;
        }
    }
    canvas.drawAll();
    m_prerendered->getTexture().bind();
    glGenerateMipmap(GL_TEXTURE_2D);

    // writeTextureToFile("../", "preRenderedFont.png", m_prerendered->getTexture());
    renderCharMapTexture();

    return true;
}

void Font::renderCharMapTexture()
{
    int char_count = m_characters.size();
    std::vector<Rectf> glyph_tex_rects(char_count);

    utils::Vector2f atlas_size = m_pixels->getSize();
    int tex_code = 0;
    for (auto &[code, character] : m_characters)
    {
        m_charcode2texcode[code] = tex_code;
        utils::Vector2f texrect_coords = {character.tex_coords.x / atlas_size.x, 1.f - character.tex_coords.y / atlas_size.y};
        utils::Vector2f texrect_size = {character.size.x / atlas_size.x, character.size.y / atlas_size.y};
        glyph_tex_rects.at(tex_code) = {texrect_coords.x, texrect_coords.y, texrect_size.x, texrect_size.y};
        tex_code++;
    }

    glGenTextures(1, &m_charmap_tex_id);
    glBindTexture(GL_TEXTURE_2D, m_charmap_tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, char_count, 1, 0, GL_RGBA, GL_FLOAT, glyph_tex_rects.data());
    glBindTexture(GL_TEXTURE_2D, 0);
    glCheckError();
}

GLuint Font::getCharmapTexId() const
{
    return m_charmap_tex_id;
}

//! \brief loads font from the specified file
//! \return true if font was succesfully loaded
bool Font::loadFromBytes(const unsigned char *bytes, std::size_t num_bytes)
{
    if (FT_Init_FreeType(mp_ft))
    {
        return false;
    }

    if (FT_New_Memory_Face(*mp_ft, bytes, num_bytes, 0, mp_face))
    {
        return false;
    }

    bool face_init_success = initializeFromFace(*mp_face);
    return face_init_success;
}

//! \brief loads font from the specified file
//! \return true if font was succesfully loaded
bool Font::loadFromFile(std::filesystem::path font_file)
{
    if (FT_Init_FreeType(mp_ft))
    {
        // spdlog::error("FREETYPE: Could not init FreeType Library");
        return false;
    }

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
    if (FT_New_Memory_Face(m_ft, buffer.data(), buffer.size(), 0, &m_face))
    {
        return false;
    }
#else
    if (FT_New_Face(*mp_ft, font_file.string().c_str(), 0, mp_face))
    {
        return false;
    }
#endif

    bool init_face_success = initializeFromFace(*mp_face);

    return init_face_success;
}

//! \return a texture containing character SDFs
Texture &Font::getTexture()
{
    return m_pixels->getTexture();
}

std::size_t Font::getFontPixelSize() const
{
    return m_font_pixel_size;
}
FreetypeMode Font::getMode() const
{
    return m_mode;
}

void Font::setFontPixelSize(std::size_t font_pixel_size)
{
    m_font_pixel_size = font_pixel_size;
    initializeFromFace(*mp_face);
}
template <class T>
std::istream &operator>>(std::istream &data, utils::Vector2<T> &vec)
{
    return data >> vec.x >> vec.y;
}
template <class T>
std::istream &operator>>(std::istream &data, Rect<T> &rect)
{
    return data >> rect.pos_x >> rect.pos_y >> rect.width >> rect.height;
}
std::istream &operator>>(std::istream &data, Character &c)
{
    return data >> c.advance >> c.bb >> c.bearing >> c.size >> c.tex_coords;
}

template <class T>
std::ostream &operator<<(std::ostream &data, const utils::Vector2<T> &vec)
{
    return data << vec.x << vec.y;
}

template <class T>
std::ostream &operator<<(std::ostream &data, const Rect<T> &rect)
{
    return data << rect.pos_x << rect.pos_y << rect.width << rect.height;
}

std::ostream &operator<<(std::ostream &data, const Character &c)
{
    return data << c.advance << c.bb << c.bearing << c.size << c.tex_coords;
}

std::unordered_map<char, Character> deserializeCharacters(const std::filesystem::path &file_path)
{
    if (!std::filesystem::exists(file_path))
    {
        std::cout << "Warning file: " << file_path << "does not exist!" << std::endl;
        return {};
    }

    std::ifstream data_file(file_path);
    std::unordered_map<char, Character> characters;

    int char_count = 0;
    data_file >> char_count;
    char code;
    Character character;
    while (data_file)
    {
        data_file >> code;
        data_file >> character;
        characters.insert({code, character});
    }

    return characters;
}
void serializeCharacters(std::ostream &data_file,
                         const std::unordered_map<int, Character> &characters)
{

    data_file << characters.size();
    for (auto [code, data] : characters)
    {
        data_file << code << data;
    }
}

void writeToFIle(const Font &font, const std::filesystem::path &path)
{
    std::ofstream data_file(path);
    data_file << font.getFontPixelSize() << static_cast<int>(font.getMode());
    serializeCharacters(data_file, font.m_characters);
}

bool Font::loadFromImage(const std::filesystem::path &image_path, const std::filesystem::path &metadata_path)
{
    deserializeCharacters(metadata_path);
    return false;
}

bool Font::containsUTF8Code(unsigned int code) const
{
    return m_characters.contains(code);
}