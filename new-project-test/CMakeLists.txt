cmake_minimum_required(VERSION 3.15)

project(projectx)

set(CMAKE_CXX_STANDARD 20)

# if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
#      include(cmake/EmscriptenFlags.cmake)
# endif()

include(FetchContent)

## RENDERER
FetchContent_Declare(
  renderer  
  GIT_REPOSITORY https://github.com/Smutekj/simple-emscripten-renderer
  GIT_TAG master
)
FetchContent_MakeAvailable(renderer)

file(GLOB SRC
"src/main.cpp"
"src/Application.h"
"src/Application.cpp"
)

add_executable(${CMAKE_PROJECT_NAME} ${SRC} )
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE 
                                             SDL2::SDL2main
                                            renderer) 


if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  set(CMAKE_EXECUTABLE_SUFFIX .html)
  target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
  "-s OFFSCREEN_FRAMEBUFFER=1"
  )
  
  target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
  "--embed-file ../external/lygia"
  "--embed-file ../Resources/Fonts"
  "--embed-file ../Resources/Textures"
  "--embed-file ../Resources/Shaders"
  )
endif()
