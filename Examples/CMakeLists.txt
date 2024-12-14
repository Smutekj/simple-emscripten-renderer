project(ExampleText)


if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
set(USE_FLAGS "-s USE_SDL=2 -s FULL_ES3=1 -s OFFSCREEN_FRAMEBUFFER=1")
set(USE_FLAGS "${USE_FLAGS} -s MIN_WEBGL_VERSION=1 -s MAX_WEBGL_VERSION=2")
set(USE_FLAGS "${USE_FLAGS} -s ASSERTIONS=0 -s ALLOW_MEMORY_GROWTH=1 -s ASYNCIFY=1")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS} --profiling --embed-file ${CMAKE_SOURCE_DIR}/Resources --embed-file ${CMAKE_SOURCE_DIR}/external/lygia")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
set(CMAKE_EXECUTABLE_SUFFIX .html)
else()
endif()


## TEXT
add_executable(ExampleText Text/main.cpp Text/Application.h Text/Application.cpp)
target_link_libraries(ExampleText SDL2::SDL2main ${CMAKE_PROJECT_NAME} )

add_executable(ExampleSprites Sprites/main.cpp Sprites/Application.h Sprites/Application.cpp)
target_link_libraries(ExampleSprites  SDL2::SDL2main ${CMAKE_PROJECT_NAME} )

add_executable(ExampleShaders Shaders/main.cpp Shaders/Application.h Shaders/Application.cpp
                Shaders/ShaderUI.h Shaders/ShaderUI.cpp )
target_link_libraries(ExampleShaders SDL2::SDL2main ${CMAKE_PROJECT_NAME} )

add_executable(ExampleLayers DrawLayers/main.cpp DrawLayers/Application.h DrawLayers/Application.cpp
            DrawLayers/DrawLayer.h DrawLayers/DrawLayer.cpp DrawLayers/PostEffects.h DrawLayers/PostEffects.cpp )
target_link_libraries(ExampleLayers SDL2::SDL2main ${CMAKE_PROJECT_NAME} )

add_executable(ExamplePostEffects PostEffects/main.cpp PostEffects/Application.h PostEffects/Application.cpp
PostEffects/DrawLayer.h PostEffects/DrawLayer.cpp PostEffects/PostEffects.h PostEffects/PostEffects.cpp )
target_link_libraries(ExamplePostEffects SDL2::SDL2main ${CMAKE_PROJECT_NAME} )

add_executable(ExampleParticles Particles/main.cpp Particles/Application.h Particles/Application.cpp
Particles/DrawLayer.h Particles/DrawLayer.cpp Particles/PostEffects.h Particles/PostEffects.cpp )
target_link_libraries(ExampleParticles SDL2::SDL2main ${CMAKE_PROJECT_NAME} )
