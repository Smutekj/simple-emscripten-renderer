#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <SDL.h>
#include <SDL_opengles2.h>
#include <GLES3/gl3platform.h>
#include <GLES3/gl3.h>
#else
#include <glad/gl.h>
#include <SDL.h>
#include <SDL_opengl.h>
#endif

#include <vector>
#include <array>
#include <numeric>
#include <iostream>

constexpr int N_MAX_TEXTURES_IN_SHADER = 2; //! maximum number of texture slots
using TextureArray = std::array<GLuint, N_MAX_TEXTURES_IN_SHADER>;

using IndexType = unsigned short;

GLenum inline glCheckError_(const char *file, int line, const char *message = "")
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        // case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break; //! these don't exist in GLES???
        // case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ") " << message << std::endl;
    }
    return errorCode;
}


//!  WebGL is super slow  with glCheckErrors
//! https://emscripten.org/docs/optimizing/Optimizing-WebGL.html

#ifdef __EMSCRIPTEN__
#define glCheckError(x) []() {}
#define glCheckError() []() {}
#define glCheckErrorMsg(x)
#else
#define glCheckErrorMsg(x) glCheckError_(__FILE__, __LINE__, x)
#define glCheckError() glCheckError_(__FILE__, __LINE__)
#endif


void inline bindVertexAttributes(GLuint buffer, std::vector<int> sizes)
{
    for (std::size_t i = 0; i < sizes.size(); ++i)
    {
        glDisableVertexAttribArray(i);
    }
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glCheckError();
    int offset = 0;
    auto total_size = std::accumulate(sizes.begin(), sizes.end(), 0);
    for (std::size_t i = 0; i < sizes.size(); ++i)
    {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, sizes.at(i), GL_FLOAT, GL_FALSE,
                              total_size * sizeof(float), (void *)(offset * sizeof(float)));
        glVertexAttribDivisor(i, 0);
        glCheckError();
        offset += sizes.at(i);
    }
}
