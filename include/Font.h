#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <filesystem>

#include <Rect.h>
#include <Utils/Vector2.h>
#include <FrameBuffer.h>
#include <Shader.h>

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

struct FontParams
{
    float smooth_min = 0.465;
    float smooth_max = 0.59;
    float glow_min = -0.1;
    float glow_max = 0.6;
    int outline = 0;
    int smooth_edges = 1;
    int outer_glow = 1;
    float outline_min0 = 0.45;
    float outline_min1 = 0.48;
    float outline_max0 = 0.495;
    float outline_max1 = 0.53;
    float thick = 0.485;
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
    Font(std::filesystem::path font_filename, std::size_t font_pixel_size = 30, FreetypeMode mode = FreetypeMode::SDF, FontParams params = {});
    Font(const unsigned char *bytes, std::size_t num_bytes, ::size_t font_pixel_size, FreetypeMode mode = FreetypeMode::SDF, FontParams params = {});
    Font(const unsigned char *prerendered_font_bytes, const char *charmap_bytes);
    Font(const unsigned char *prerendered_font_bytes, std::size_t num_bytes);
    ~Font();

    bool containsUTF8Code(unsigned int) const;
    bool loadFromFile(std::filesystem::path font_filename);
    bool loadFromBytes(const unsigned char *bytes, std::size_t num_bytes);
    bool loadFromImage(const std::filesystem::path &image_path, const std::filesystem::path &metadata_path);
    bool loadCharMapFromBytes(const unsigned char *bytes, std::size_t num_bytes);
    Texture &getTexture();

    std::size_t getFontPixelSize() const;
    void setFontPixelSize(std::size_t font_pixe_size);
    FreetypeMode getMode() const;

    float getLineHeight() const;

    GLuint getCharmapTexId() const;

    void writeToFile(const std::filesystem::path &path);

    Shader &getShader();
    
    void setParams(FontParams params);
    FontParams getParams()const;

private:
    void deserializeCharacters(const unsigned char *charmap_bytes, std::size_t num_bytes);
    void renderCharMapTexture();
    bool initializeFromFace(FT_Face &face);

public:
    std::shared_ptr<Texture> m_texture;              //!< stores a texture with all characters that we draw from when drawing text
    std::unordered_map<int, Character> m_characters; //!< stores all Glyph data of respective characters
    std::unordered_map<int, int> m_charcode2texcode;

private:
    std::unique_ptr<FrameBuffer> m_prerendered;
    std::unique_ptr<FrameBuffer> m_charmap_texture;

    FreetypeMode m_mode = FreetypeMode::Normal;
    std::size_t m_font_pixel_size = 20;
    float m_line_height;

    GLuint m_charmap_tex_id = 0;

    FontParams m_params;
    Shader m_shader;

    //! FT handles
    std::unique_ptr<FT_Face> mp_face;
    std::unique_ptr<FT_Library> mp_ft;
};
