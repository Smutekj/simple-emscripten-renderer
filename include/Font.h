#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
// #include FT_GLYPH_H   //optional glyph management component
// #include FT_OUTLINE_H //scalable outline management
// #include FT_STROKER_H //functions to stroke outline paths

#include <Utils/Vector2.h>
#include <FrameBuffer.h>

struct Character
{
    unsigned int texture_id; // ID handle of the glyph texture
    utils::Vector2i tex_coords;
    utils::Vector2i size;    // Size of glyph
    utils::Vector2i bearing; // Offset from baseline to left/top of glyph
    unsigned int advance;    // Offset to advance to next glyph
};

class Renderer;

class Font
{
public:
    Font(std::string font_filename);
    bool loadFromFile(std::string font_filename);
    Texture& getTexture();

private:
    void initialize();

public:
    std::unique_ptr<FrameBuffer> m_pixels;
    Texture m_texture;
    std::unordered_map<char, Character> m_characters;
private:
    std::unique_ptr<Renderer> m_canvas;
};
