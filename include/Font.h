#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
// #include FT_GLYPH_H   //optional glyph management component (I keep these here because I'll probably need them )
// #include FT_OUTLINE_H //scalable outline management
// #include FT_STROKER_H //functions to stroke outline paths


//! \struct Character
//! \brief holds Freetype character dimensions and relative position data
struct Character
{
    unsigned int texture_id; // ID handle of the glyph texture
    utils::Vector2i tex_coords;
    utils::Vector2i size;    // Size of glyph
    utils::Vector2i bearing; // Offset from baseline to left/top of glyph
    unsigned int advance;    // Offset to advance to next glyph
};

class Renderer;
class FrameBuffer;

//! \class Font
//! \brief stores all data related to a given fotn
//! \brief stores information necessary for drawing for each character in the font;
//! \brief also contains texture, which contains all glyphs
class Font
{
public:
    Font(std::filesystem::path font_filename);
    bool loadFromFile(std::filesystem::path font_filename);
    Texture& getTexture();

public:
    std::unique_ptr<FrameBuffer> m_pixels;  //!< stores a framebuffer to draw all characters into (Do i need to store it?)
    Texture m_texture;  //!< stores a texture with all characters that we draw from when drawing text
    std::unordered_map<char, Character> m_characters; //!< stores all Glyph data of respective characters
private:
    std::unique_ptr<Renderer> m_canvas;  
};
