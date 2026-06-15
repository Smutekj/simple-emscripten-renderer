#include "GLSLOptimizer.h"

#include <iostream>

#include <glsl_optimizer.h>

namespace glsl_optimizer
{
    static glslopt_ctx *s_glslopt_ctx = nullptr;

    static std::string optimizeGlsl(const std::string &shader_src, glslopt_shader_type type)
    {

        //! only optimize on android
#if !defined(ANDROID)
        return shader_src;
#endif
        glslopt_shader *shader =
            glslopt_optimize(s_glslopt_ctx,
                             type,
                             shader_src.c_str(), 0);

        std::string new_shader_src = shader_src;

        if (glslopt_get_status(shader))
        {
            new_shader_src = glslopt_get_output(shader);
        }
        else
        {
            std::cout << glslopt_get_log(shader) << std::endl;
        }

        glslopt_shader_delete(shader);
        return new_shader_src;
    }
    std::string optimizeFragmentShader(const std::string &source)
    {
        return optimizeGlsl(source, glslopt_shader_type::kGlslOptShaderFragment);
    }
    std::string optimizeVertexShader(const std::string &source)
    {
        return optimizeGlsl(source, glslopt_shader_type::kGlslOptShaderVertex);
    }

    void init()
    {
        constexpr glslopt_target s_glslopt_target_version = glslopt_target::kGlslTargetOpenGLES30;
        s_glslopt_ctx = glslopt_initialize(s_glslopt_target_version);
    }

    void deinit()
    {
        glslopt_cleanup(s_glslopt_ctx);
    }

} // glsl_optimizer
