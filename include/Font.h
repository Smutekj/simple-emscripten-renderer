#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <filesystem>

#include <Rect.h>
#include <Utils/Vector2.h>
#include <FrameBuffer.h>

//! \struct Character
//! \brief holds Freetype character dimensions and relative position data
struct Character
{
    unsigned int texture_id; // ID handle of the glyph texture
    utils::Vector2i tex_coords;
    utils::Vector2i size;    // Size of glyph
    utils::Vector2i bearing; // Offset from baseline to left/top of glyph
    Rectf bb;
    unsigned int advance; // Offset to advance to next glyph
};

class Renderer;
class FrameBuffer;
class Texture;

enum class FreetypeMode
{
    Normal = 0,
    SDF = 5,
};

typedef struct FT_LibraryRec_ *FT_Library;
typedef struct FT_FaceRec_ *FT_Face;

//! \class Font
//! \brief stores all data related to a given fotn
//! \brief stores information necessary for drawing for each character in the font;
//! \brief also contains texture, which contains all glyphs
class Font
{
public:
    Font(std::filesystem::path font_filename, std::size_t font_pixel_size = 30, FreetypeMode mode = FreetypeMode::SDF);
    Font(const unsigned char *bytes, std::size_t num_bytes, ::size_t font_pixel_size = 30, FreetypeMode mode = FreetypeMode::SDF);
    Font(Texture &font_texture, std::unordered_map<int, Character> &char_map);
    ~Font();

    bool containsUTF8Code(unsigned int)const;
    bool loadFromFile(std::filesystem::path font_filename);
    bool loadFromBytes(const unsigned char *bytes, std::size_t num_bytes);
    bool loadFromTexture(Texture &texture);
    bool loadFromImage(const std::filesystem::path &image_path, const std::filesystem::path &metadata_path);
    Texture &getTexture();

    std::size_t getFontPixelSize() const;
    void setFontPixelSize(std::size_t font_pixe_size);
    FreetypeMode getMode() const;

    float getLineHeight() const;

    GLuint getCharmapTexId() const;

    void saveToFile(std::filesystem::path font_image, const std::string &font_name);

private:
    void renderCharMapTexture();
    bool initializeFromFace(FT_Face &face);

public:
    std::unique_ptr<FrameBuffer> m_pixels;           //!< stores a framebuffer to draw all characters into (Do i need to store it?)
    Texture *p_texture = nullptr;                    //!< stores a texture with all characters that we draw from when drawing text
    std::unordered_map<int, Character> m_characters; //!< stores all Glyph data of respective characters
    std::unordered_map<int, int> m_charcode2texcode;

private:
    std::unique_ptr<FrameBuffer> m_prerendered;
    std::unique_ptr<FrameBuffer> m_charmap_texture;

    FreetypeMode m_mode = FreetypeMode::Normal;
    std::size_t m_font_pixel_size = 20;
    float m_line_height;

    std::unique_ptr<Renderer> m_canvas;
    
    GLuint m_charmap_tex_id = 0;

    //! FT handles
    std::unique_ptr<FT_Face> mp_face;
    std::unique_ptr<FT_Library> mp_ft;
};
