#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "gtest/gtest.h"

 #include "test_context.cc"
 #include "test_shader.cc"

int main(int argc, char **argv)
{
  // this is here because we have our own main
  SDL_SetMainReady();
 
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}