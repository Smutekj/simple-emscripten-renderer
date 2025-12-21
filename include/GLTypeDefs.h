#pragma once

#include <array>
using GLuint = unsigned int;
using GLint = int;
using GLenum = unsigned int;

constexpr int N_MAX_TEXTURES_IN_SHADER = 2; //! maximum number of texture slots
using TextureArray = std::array<GLuint, N_MAX_TEXTURES_IN_SHADER>;

//! \enum DrawType
//! \brief Corresponds to OpenGL draw buffers needed in glBufferData
enum class DrawType
{
    Dynamic,
    Static,
    Stream,
};
GLint getGLCode(DrawType type);

//! \enum BlendFactor
//! \brief a factor in the color blending equation
//! for details see: \ref https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glBlendFuncSeparate.xhtml
enum class BlendFactor
{
    One,
    Zero,
    SrcAlpha,
    OneMinusSrcAlpha,
    SrcColor,
};
GLint getGLCode(BlendFactor factor);

using TextureHandle = unsigned int;
//! \enum TexMappingParam
//! \brief options for texture mapping procedure
//! translates into corresponding OpenGL integers explanations are here:
//! \ref https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glTexParameter.xhtml
enum class TexMappingParam
{
    Nearest,
    Linear,
    NearestMipmapLinear,
    NearestMipmapNearest,
    LinearMipmapLinear,
    LinearMipmapNearest,
};
GLint getGLCode(TexMappingParam p);

//! \enum TextureDataTypes
//! \brief options for data types stored in texture pixels
//! \ref https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glTexParameter.xhtml
enum class TextureDataTypes
{
    Float,
    UByte,
};
GLint getGLCode(TextureDataTypes p);

//! \enum TextureFormat
//! \brief format of the data stored in each texture pixel
//! this will influences the data format of the texture pixels in shaders
//! \ref https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glTexParameter.xhtml
enum class TextureFormat
{
    RGBA,
    Red,
    RGBA16F,
    RGBA32F,
};
GLint getGLCode(TextureFormat p);

//! \enum TexWrapParam
//! \brief specifies the boundary condition used in drawing textures
//! \ref https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glTexParameter.xhtml
enum class TexWrapParam
{
    Repeat,   //!< uses periodic boundary conditions in texture space (cool for making patters)
    ClampEdge //!< uses fixed value boundary condition in texture space, if the
};
GLint getGLCode(TexWrapParam p);

enum class ShaderType
{
    Vertex,
    Fragment,
    Geometry,
    TessControl,
    TessEvaluation,
    Compute,
};

GLenum getGLCode(ShaderType type);