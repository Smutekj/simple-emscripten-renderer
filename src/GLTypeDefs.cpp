#include "GLTypeDefs.h"

#include "IncludesGl.h"

GLint getGLCode(DrawType type)
{
    switch (type)
    {
    case DrawType::Dynamic:
        return GL_DYNAMIC_DRAW;
    case DrawType::Static:
        return GL_STATIC_DRAW;
    case DrawType::Stream:
        return GL_STREAM_DRAW;
    }
    return 0;
}

GLint getGLCode(BlendFactor factor)
{
    switch (factor)
    {
    case BlendFactor::One:
        return GL_ONE;
    case BlendFactor::Zero:
        return GL_ZERO;
    case BlendFactor::OneMinusSrcAlpha:
        return GL_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::SrcAlpha:
        return GL_SRC_ALPHA;
    case BlendFactor::SrcColor:
        return GL_SRC_COLOR;
    }
    return 0;
}

GLint getGLCode(TexMappingParam p)
{
    using pm = TexMappingParam;
    switch (p)
    {
    case pm::Nearest:
        return GL_NEAREST;
    case pm::Linear:
        return GL_LINEAR;
    case pm::LinearMipmapLinear:
        return GL_LINEAR_MIPMAP_LINEAR;
    case pm::NearestMipmapNearest:
        return GL_NEAREST_MIPMAP_NEAREST;
    case pm::LinearMipmapNearest:
        return GL_LINEAR_MIPMAP_NEAREST;
    case pm::NearestMipmapLinear:
        return GL_NEAREST_MIPMAP_LINEAR;
    }
    return 0;
}

GLint getGLCode(TextureDataTypes p)
{
    using pm = TextureDataTypes;
    switch (p)
    {
    case (pm::Float):
        return GL_FLOAT;
    case (pm::UByte):
        return GL_UNSIGNED_BYTE;
    }
    return 0;
}

GLint getGLCode(TextureFormat p)
{
    using pm = TextureFormat;
    switch (p)
    {
    case (pm::Red):
        return GL_RED;
    case (pm::R8):
        return GL_R8;
    case (pm::RGBA):
        return GL_RGBA;
    case (pm::RGBA16F):
        return GL_RGBA16F;
    case (pm::RGBA32F):
        return GL_RGBA32F;
    }
    return 0;
}

GLint getGLCode(TexWrapParam p)
{
    using pm = TexWrapParam;
    switch (p)
    {
    case (pm::Repeat):
        return GL_REPEAT;
    case (pm::ClampEdge):
        return GL_CLAMP_TO_EDGE;
    }
    return 0;
}

GLenum getGLCode(ShaderType type)
{
    using st = ShaderType;
    switch (type)
    {
    case st::Vertex:
        return GL_VERTEX_SHADER;
    case st::Fragment:
        return GL_FRAGMENT_SHADER;
#ifndef __EMSCRIPTEN__
    case st::Geometry:
        return GL_GEOMETRY_SHADER;
    case st::TessControl:
        return GL_TESS_CONTROL_SHADER;
    case st::TessEvaluation:
        return GL_TESS_EVALUATION_SHADER;
    case st::Compute:
        return GL_COMPUTE_SHADER;
#endif
    };
    return 0;
}