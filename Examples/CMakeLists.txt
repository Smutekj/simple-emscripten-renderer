project(ExampleText)

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
		--embed-file ${CMAKE_SOURCE_DIR}/external/lygia \
		--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Fonts@/../Resources/Fonts \
		--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Shaders@/../Resources/Shaders \
		--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Textures@/../Resources/Textures \
		")
endif()

## TEXT
add_executable(ExampleText Text/main.cpp Text/Application.h Text/Application.cpp)
target_link_libraries(ExampleText SDL2::SDL2main ${CMAKE_PROJECT_NAME} )

add_executable(ExampleSprites Sprites/main.cpp Sprites/Application.h Sprites/Application.cpp)
target_link_libraries(ExampleSprites  SDL2::SDL2main ${CMAKE_PROJECT_NAME} )

add_executable(ExampleVertexArray VertexArray/main.cpp VertexArray/Application.h VertexArray/Application.cpp)
target_link_libraries(ExampleVertexArray  SDL2::SDL2main ${CMAKE_PROJECT_NAME} )

add_executable(ExampleShaders Shaders/main.cpp Shaders/Application.h Shaders/Application.cpp
                Shaders/ShaderUI.h Shaders/ShaderUI.cpp )
target_link_libraries(ExampleShaders SDL2::SDL2main ${CMAKE_PROJECT_NAME} )

add_executable(ExampleLayers DrawLayers/main.cpp DrawLayers/Application.h DrawLayers/Application.cpp)
target_link_libraries(ExampleLayers SDL2::SDL2main ${CMAKE_PROJECT_NAME} )

add_executable(ExamplePostEffects PostEffects/main.cpp PostEffects/Application.h PostEffects/Application.cpp )
target_link_libraries(ExamplePostEffects SDL2::SDL2main ${CMAKE_PROJECT_NAME} )

add_executable(ExampleParticles Particles/main.cpp Particles/Application.h Particles/Application.cpp)
target_link_libraries(ExampleParticles SDL2::SDL2main ${CMAKE_PROJECT_NAME} )

set_target_properties(ExampleText ExampleSprites ExampleVertexArray ExampleShaders ExampleLayers ExamplePostEffects ExampleParticles
						PROPERTIES 
						RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
					)
