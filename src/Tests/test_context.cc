#include <gtest/gtest.h>
#include <Window.h>
#include "IncludesGl.h"

namespace
{

     TEST(TestContext, GladFunctionLoad)
     {
          int width = 800;
          int height = 600;

         SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
         SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
         SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

         // Create SDL window
         SDL_Window *m_handle =
             SDL_CreateWindow("Test",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

         auto m_gl_context = SDL_GL_CreateContext(m_handle);

         int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

         EXPECT_TRUE(version != 0);

         SDL_GL_DeleteContext(m_gl_context);
         SDL_DestroyWindow(m_handle);
         SDL_Quit();
     }

     TEST(TestContext, VersionCreation)
     {
         int width = 800;
         int height = 600;

         SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
         SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
         SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

         // Create SDL window
         SDL_Window *m_handle =
             SDL_CreateWindow("Test",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

         auto m_gl_context = SDL_GL_CreateContext(m_handle);

         gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

         int major_version;
         int minor_version;
         glGetIntegerv(GL_MAJOR_VERSION, &major_version);
         glGetIntegerv(GL_MINOR_VERSION, &minor_version);

         std::cout << "Created version: " << major_version << "." << minor_version << "\n";

         EXPECT_TRUE(major_version >= 3) << "Major GL version too low!";

         SDL_GL_DeleteContext(m_gl_context);
         SDL_DestroyWindow(m_handle);
         SDL_Quit();
     }

     TEST(TestContext, GLFunctionLoad)
     {
         int width = 800;
         int height = 600;

         SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
         SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
         SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

         // Create SDL window
         SDL_Window *m_handle =
             SDL_CreateWindow("Test",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

         auto m_gl_context = SDL_GL_CreateContext(m_handle);

         gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

         EXPECT_TRUE(glGetString != nullptr) << "glGetString not available!";
         EXPECT_TRUE(glCreateBuffers != nullptr) << "glCreateBuffers not available!";
         EXPECT_TRUE(glCreateFramebuffers != nullptr) << "glCreateFramebuffers not available!";
         EXPECT_TRUE(glCreateVertexArrays != nullptr) << "glCreateVertexArrays not available!";
         EXPECT_TRUE(glCreateVertexArrays != nullptr) << "glCreateVertexArrays not available!";
         EXPECT_TRUE(glBindBuffer != nullptr) << "glBindBuffer not available!";
         EXPECT_TRUE(glTexImage2D != nullptr) << "glTexImage2D not available!";
         EXPECT_TRUE(glDrawArrays != nullptr) << "glDrawArrays not available!";
         EXPECT_TRUE(glDrawArraysInstanced != nullptr) << "glDrawArraysInstanced not available!";
         EXPECT_TRUE(glCreateProgram != nullptr) << "glCreateProgram not available!";
         EXPECT_TRUE(glLinkProgram != nullptr) << "glLinkProgram not available!";

         SDL_GL_DeleteContext(m_gl_context);
         SDL_DestroyWindow(m_handle);
         SDL_Quit();
     }

}