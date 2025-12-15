#include "BlendParams.h"

#include "IncludesGl.h"

BlendParams::BlendParams(BlendFactor src_fact, BlendFactor dst_fact)
    : src_factor(src_fact), dst_factor(dst_fact)
{
}
BlendParams::BlendParams(BlendFactor src_fact, BlendFactor dst_fact, BlendFactor src_a, BlendFactor dst_a)
    : src_factor(src_fact), dst_factor(dst_fact), src_alpha(src_a), dst_alpha(dst_a)
{
}


//! \brief does GL calls for setting the blend function parameters
//! \param params   OpenGL blend equation parameters
void setBlendParams(BlendParams params)
{
    auto df = getGLCode(params.dst_factor);
    auto da = getGLCode(params.dst_alpha);
    auto sf = getGLCode(params.src_factor);
    auto sa = getGLCode(params.src_alpha);

    glBlendFuncSeparate(sf, df, sa, da);
    glCheckErrorMsg("Error in glBlendFuncSeparate!");
}
