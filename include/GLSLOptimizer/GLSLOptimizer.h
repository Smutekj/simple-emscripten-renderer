#pragma once

#include <string>

namespace glsl_optimizer
{
    std::string optimizeFragmentShader(const std::string &source);
    std::string optimizeVertexShader(const std::string &source);
    
    void init();
    void deinit();

} // glsl_optimizer
