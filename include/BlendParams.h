#pragma once

#include "GLTypeDefs.h"

//! \struct BlendParams
//! \brief holds info for color blending for OpenGL
//!  the order of parameters corresponds to the order in glBlendFunSeparate(...);
//! for details see: \ref https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glBlendFuncSeparate.xhtml
struct BlendParams
{
    BlendFactor src_factor = BlendFactor::One;
    BlendFactor dst_factor = BlendFactor::OneMinusSrcAlpha;
    BlendFactor src_alpha = BlendFactor::One;
    BlendFactor dst_alpha = BlendFactor::OneMinusSrcAlpha;

    BlendParams() = default;
    BlendParams(BlendFactor src_fact, BlendFactor dst_fact);
    BlendParams(BlendFactor src_fact, BlendFactor dst_fact, BlendFactor src_a, BlendFactor dst_a);
};

//! \brief Calls OpenGL function which sets the blending function parameters
//! \param parameters 
void setBlendParams(BlendParams parameters);