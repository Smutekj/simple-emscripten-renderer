#include "Font.h"

#include <fstream>
#include <cstring>
#include <unordered_set>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H   //optional glyph management component (I keep these here because I'll probably need them )
#include FT_OUTLINE_H //scalable outline management
// #include FT_STROKER_H //functions to stroke outline paths

#include "IncludesGl.h"
#include "CommonShaders.inl"
#include "Renderer.h"
#include "FrameBuffer.h"
#include "Sprite.h"

Font::~Font()
{
    FT_Done_Face(*mp_face);
    FT_Done_FreeType(*mp_ft);
}

//! \brief creates a font from a path to a file
//! \param font_filename path to a font file
Font::Font(std::filesystem::path font_filename,
           size_t font_pixel_size,
           FreetypeMode mode,
           FontParams params)
    : m_mode(mode),
      m_font_pixel_size(font_pixel_size),
      m_params(params)
{
    mp_face = std::make_unique<FT_Face>(FT_Face());
    mp_ft = std::make_unique<FT_Library>(FT_Library());

    if (!loadFromFile(font_filename))
    {
        throw std::runtime_error("FONT FILE " + font_filename.string() + " NOT FOUND!");
    }

    if (m_mode == FreetypeMode::SDF)
    {
        m_shader.loadFromCode(std::string{vertex_text_code}, std::string{fragment_text_sdf_code});
        setParams(m_params);
    }
    else
    {
        m_shader.loadFromCode(std::string{vertex_text_code}, std::string{fragment_text_sprite_code});
    }
}

Font::Font(const unsigned char *bytes,
           std::size_t num_bytes,
           size_t font_pixel_size,
           FreetypeMode mode,
           FontParams params)
    : m_mode(mode),
      m_font_pixel_size(font_pixel_size),
      m_params(params)
{
    mp_face = std::make_unique<FT_Face>(FT_Face());
    mp_ft = std::make_unique<FT_Library>(FT_Library());

    auto tic = std::chrono::high_resolution_clock::now();
    if (!loadFromBytes(bytes, num_bytes))
    {
        throw std::runtime_error("UNABLE TO LOAD FONT");
    }

    if (m_mode == FreetypeMode::SDF)
    {
        m_shader.loadFromCode(std::string{vertex_text_code}, std::string{fragment_text_sdf_code});
        setParams(m_params);
    }
    else
    {
        m_shader.loadFromCode(std::string{vertex_text_code}, std::string{fragment_text_sprite_code});
    }
}

Font::Font(const unsigned char *prerendered_font_bytes, std::size_t num_bytes)
{
    TextureOptions options;
    options.data_type = TextureDataTypes::UByte;
    options.format = TextureFormat::RGBA;
    options.internal_format = TextureFormat::RGBA;
    options.mag_param = TexMappingParam::Linear;
    options.min_param = TexMappingParam::Linear;
    m_texture = std::make_unique<Texture>(prerendered_font_bytes, num_bytes, options);
}

Font::Font(const unsigned char *prerendered_font_bytes, const char *charmap_bytes)
{
    assert(false);
    // m_texture = std::make_unique<Texture>(prerendered_font_bytes, TextureOptions{});
    // deserializeCharacters(charmap_bytes);
}

Shader &Font::getShader()
{
    return m_shader;
}

void Font::setParams(FontParams params)
{
    m_params = params;

    m_shader.setUniform("u_smooth_min", params.smooth_min);
    m_shader.setUniform("u_smooth_max", params.smooth_max);
    m_shader.setUniform("u_glow_min", params.glow_min);
    m_shader.setUniform("u_glow_max", params.glow_max);
    m_shader.setUniform("u_outline", params.outline);
    m_shader.setUniform("u_smooth_edges", params.smooth_edges);
    m_shader.setUniform("u_outer_glow", params.outer_glow);
    m_shader.setUniform("u_outline_min0", params.outline_min0);
    m_shader.setUniform("u_outline_min1", params.outline_min1);
    m_shader.setUniform("u_outline_max0", params.outline_max0);
    m_shader.setUniform("u_outline_max1", params.outline_max1);
    m_shader.setUniform("u_thick", params.thick);
}
FontParams Font::getParams() const
{
    return m_params;
}

bool Font::loadCharMapFromBytes(const unsigned char *charmap_bytes, std::size_t num_bytes)
{
    deserializeCharacters(charmap_bytes, num_bytes);
    return true;
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
    std::size_t atlas_w = 2048; //! how to set this?
    std::size_t atlas_h = 2048;
    std::size_t safety_margin = 4; //! number of pixels that separate glyphs in texture
    utils::Vector2<unsigned int> max_char_size = {0, 0};

    //! create texture to draw glyphs into
    TextureOptions atlas_texture_options = {
        .format = TextureFormat::Red,
        .internal_format = TextureFormat::R8,
        .data_type = TextureDataTypes::UByte,
        .mag_param = TexMappingParam::Nearest,
        .min_param = TexMappingParam::Nearest,
        .wrap_x = TexWrapParam::ClampEdge,
        .wrap_y = TexWrapParam::ClampEdge,
        .mipmap_levels = 0};
    m_texture = std::make_shared<Texture>(atlas_w, atlas_h, atlas_texture_options);

    FT_Set_Pixel_Sizes(face, 0, m_font_pixel_size);
    m_line_height = face->size->metrics.height / 64.f;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    glCheckError();

    m_texture->bind();
    //! zero the texture
    std::vector<uint8_t> zeros(atlas_h * atlas_w, 0);
    glTexImage2D(GL_TEXTURE_2D, 0,
                 getGLCode(atlas_texture_options.internal_format),
                 atlas_w, atlas_h,
                 0,
                 getGLCode(atlas_texture_options.format),
                 getGLCode(atlas_texture_options.data_type),
                 zeros.data());
    glCheckError();

    //! initialize characters data
    m_characters.clear();
    utils::Vector2i glyph_pos = {safety_margin, safety_margin};

    std::unordered_set<int> charset = {'$', '%', '@', ':', ',', '(', ')', '=', '-', '+', '*', '/',
                                       'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', ' ',
                                       'i', 'j', 'k', 'l',
                                       'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
                                       'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
                                       'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
                                       'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7',
                                       '8', '9', '!', '.', '?'};

    FT_UInt gindex;
    FT_ULong charcode = FT_Get_First_Char(face, &gindex);
    while (gindex != 0)
    {
        // load character glyph
        if (!charset.contains(charcode))
        {
            charcode = FT_Get_Next_Char(face, charcode, &gindex);
            continue;
        }

        if (FT_Load_Char(face, charcode, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        FT_GlyphSlot &glyph = face->glyph;
        FT_Bitmap &bitmap = glyph->bitmap;
        FT_Render_Glyph(glyph, static_cast<FT_Render_Mode>(m_mode));

        if (glyph_pos.x + bitmap.width + safety_margin >= atlas_w)
        {
            //! if we reach right side of the main texture
            glyph_pos.y += max_char_size.y + safety_margin;
            glyph_pos.x = 0;
            max_char_size = {0, 0};
        }
        max_char_size = {std::max(bitmap.width, max_char_size.x),
                         std::max(bitmap.rows, max_char_size.y)};

        glTexSubImage2D(GL_TEXTURE_2D, 0,
                        glyph_pos.x, glyph_pos.y,
                        bitmap.width, bitmap.rows,
                        GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);
        glCheckError(); //! the error here is most likely due to rendering outside of texture

        FT_BBox bbox;
        FT_Outline_Get_CBox(&glyph->outline, &bbox);

        float width = (float)(bbox.xMax / 64 - bbox.xMin / 64);
        float height = (float)(bbox.yMax / 64 - bbox.yMin / 64);
        Rectf char_bb = {(float)(bbox.xMin / 64), (float)(bbox.yMin / 64), width, height};

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
                // main_texture.getHandle(),
                m_texture->getHandle(),
                glyph_pos,
                {bitmap.width, bitmap.rows},
                {glyph->bitmap_left, glyph->bitmap_top},
                char_bb,
                (unsigned int)glyph->advance.x};
        m_characters.insert(std::pair<int, Character>(charcode, character));

        glyph_pos.x += bitmap.width + safety_margin; //! move position to next glyph

        charcode = FT_Get_Next_Char(face, charcode, &gindex);
    }

    m_texture->setMappingMinify(TexMappingParam::Linear);
    m_texture->setMappingMagnify(TexMappingParam::Linear);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); //! set back to deafult value

    renderCharMapTexture();
    return true;
}

//! \brief renders glyph information into texture which will be read from gpu by shader
void Font::renderCharMapTexture()
{
    int char_count = m_characters.size();
    std::vector<Rectf> glyph_tex_rects(char_count);

    utils::Vector2f atlas_size = Vec2{m_texture->getSize()};
    int tex_code = 0;
    for (auto &[code, character] : m_characters)
    {
        m_charcode2texcode[code] = tex_code;
        utils::Vector2f texrect_coords = {character.tex_coords.x / atlas_size.x,
                                          character.tex_coords.y / atlas_size.y};
        utils::Vector2f texrect_size = {character.size.x / atlas_size.x,
                                        character.size.y / atlas_size.y};
        glyph_tex_rects.at(tex_code) = {texrect_coords.x, texrect_coords.y, texrect_size.x,
                                        texrect_size.y};
        tex_code++;
    }

    glGenTextures(1, &m_charmap_tex_id);
    glBindTexture(GL_TEXTURE_2D, m_charmap_tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, char_count, 1, 0, GL_RGBA, GL_FLOAT,
                 glyph_tex_rects.data());
    glBindTexture(GL_TEXTURE_2D, 0);
    glCheckError();

    //    writeToFile("../Resources/Fonts/");
}

GLuint Font::getCharmapTexId() const
{
    return m_charmap_tex_id;
}

//! \brief loads font from the specified file
//! \return true if font was succesfully loaded
bool Font::loadFromBytes(const unsigned char *bytes, std::size_t num_bytes)
{
    if (FT_Init_FreeType(mp_ft.get()))
    {
        return false;
    }

    if (FT_New_Memory_Face(*mp_ft, bytes, num_bytes, 0, mp_face.get()))
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
    if (FT_Init_FreeType(mp_ft.get()))
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
    if (FT_New_Memory_Face(*mp_ft, buffer.data(), buffer.size(), 0, mp_face.get()))
    {
        return false;
    }
#else
    if (FT_New_Face(*mp_ft, font_file.string().c_str(), 0, mp_face.get()))
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
    return *m_texture;
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

void Font::deserializeCharacters(const unsigned char *charmap_bytes, std::size_t num_bytes)
{
    const unsigned char *ptr = charmap_bytes;

    std::memcpy(&m_font_pixel_size, ptr, sizeof(m_font_pixel_size));
    ptr += sizeof(m_font_pixel_size);

    std::memcpy(&m_line_height, ptr, sizeof(m_line_height));
    ptr += sizeof(m_line_height);

    std::size_t char_count;
    std::memcpy(&char_count, ptr, sizeof(char_count));
    ptr += sizeof(char_count);

    for (std::size_t i = 0; i < char_count; ++i)
    {
        int code;
        Character character;

        std::memcpy(&code, ptr, sizeof(code));
        ptr += sizeof(code);

        std::memcpy(&character, ptr, sizeof(character));
        ptr += sizeof(character);

        m_characters.insert({code, character});
    }

    renderCharMapTexture();
}

void Font::writeToFile(const std::filesystem::path &path)
{
    std::ofstream data_file(path.string() + "test.dat", std::ios::binary);
    data_file.write(reinterpret_cast<const char *>(&m_font_pixel_size), sizeof(m_font_pixel_size));
    data_file.write(reinterpret_cast<const char *>(&m_line_height), sizeof(m_line_height));
    // data_file.write(reinterpret_cast<const char *>(m_mode), sizeof(m_mode));
    std::size_t char_count = m_characters.size();
    data_file.write(reinterpret_cast<const char *>(&char_count), sizeof(m_characters.size()));
    for (auto [code, data] : m_characters)
    {
        data_file.write(reinterpret_cast<const char *>(&code), sizeof(code));
        data_file.write(reinterpret_cast<const char *>(&data), sizeof(data));
        // data_file << data;
        // // data_file.write(reinterpret_cast<const char *>(data), sizeof(data));
        // data_file << code << data;
    }
    data_file.close();

    writeTextureToFile(path, "-font.png", *m_texture);
}

bool Font::loadFromImage(const std::filesystem::path &pre_rendered_font_path,
                         const std::filesystem::path &metadata_path)
{
    auto tex = std::make_shared<Texture>(pre_rendered_font_path.string(), TextureOptions{});
    // m_characters = deserializeCharacters(metadata_path);
    renderCharMapTexture();
    return false;
}

bool Font::containsUTF8Code(unsigned int code) const
{
    return m_characters.contains(code);
}

//! prerendered font just in case i need it....
// TextureOptions text_options;
// text_options.data_type = TextureDataTypes::UByte;
// text_options.format = TextureFormat::RGBA;
// text_options.internal_format = TextureFormat::RGBA;
// text_options.mag_param = TexMappingParam::Linear;
// text_options.min_param = TexMappingParam::Linear;
// m_prerendered = std::make_unique<FrameBuffer>(atlas_w, atlas_h, text_options);
// Renderer canvas(*m_prerendered);
// glyph_pos = {0.f, m_font_pixel_size};
// float max_image_height = 0;
// canvas.clear({0, 0, 0, 0});
// canvas.m_view = canvas.getDefaultView();
// Sprite glyph(getTexture());
// glyph.setColor({255, 255, 255, 255});
// for (auto &[code, character] : m_characters)
// {
//     float glyph_width = character.size.x * 2;
//     float glyph_height = character.size.y * 2;
//     float dy = character.size.y - character.bearing.y;

//     utils::Vector2f pos = glyph_pos + Vec2{character.bearing.x + glyph_width / 2.f, glyph_height / 2.f - dy};
//     glyph.m_tex_rect = {character.tex_coords.x, character.tex_coords.y,
//                         character.size.x, character.size.y};

//     glyph.setPosition(pos);
//     glyph.setScale(glyph_width / 2., glyph_height / 2.);
//     auto image_height = character.bb.height * 2;
//     auto image_width = character.bb.width * 2;
//     max_image_height = std::max(image_height, max_image_height);
//     canvas.drawSprite(glyph, "TextDefault");

//     glyph_pos.x += (character.advance >> 6) * 2;
//     if (glyph_pos.x + image_width >= atlas_w)
//     {
//         glyph_pos.x = 0;
//         glyph_pos.y += max_image_height;
//     }
// }
// canvas.drawAll();
// m_prerendered->getTexture().bind();
// glGenerateMipmap(GL_TEXTURE_2D);
// writeTextureToFile("../", "preRenderedFont.png", m_prerendered->getTexture())