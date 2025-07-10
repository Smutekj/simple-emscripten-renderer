cmake_minimum_required(VERSION 3.15)

project(projectx)

set(CMAKE_CXX_STANDARD 20)

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
  target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC "-sNO_DISABLE_EXCEPTION_CATCHING=1")
  target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
        "-sUSE_SDL=2"
        "-sUSE_WEBGL2=1"
        "-sFULL_ES3=1"
        "-sMIN_WEBGL_VERSION=2"
        "-sMAX_WEBGL_VERSION=2"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sOFFSCREEN_FRAMEBUFFER=1"
        "-sEXCEPTION_CATCHING_ALLOWED=yes"
    )

  #### DO NOT PUT QUOTATION MARKS IN HERE!!! YOU WILL LOSE ALL HOPE!!! (THESE ARE QUOTATION MARSK: "")
  target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
  --embed-file ${CMAKE_CURRENT_SOURCE_DIR}/external/lygia@external/lygia 
  --embed-file ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Textures@Resources/Textures
  --embed-file ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Fonts@Resources/Fonts
  --embed-file ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Shaders@Resources/Shaders
  )
endif()
