cmake_minimum_required(VERSION 3.15)


project(renderer)
option(BUILD_EXAMPLE OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

if(NOT CMAKE_BUILD_TYPE) 
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()


include(FetchContent)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG master
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL
  GIT_TAG SDL2
)
FetchContent_MakeAvailable(SDL2)

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
set(USE_FLAGS "-s USE_SDL=2 -s USE_FREETYPE=1 -s FULL_ES3=1 -s OFFSCREEN_FRAMEBUFFER=1")
set(USE_FLAGS "${USE_FLAGS} -s MIN_WEBGL_VERSION=1 -s MAX_WEBGL_VERSION=2")
# set(USE_FLAGS "${USE_FLAGS} -s -sASSERTIONS -sALLOW_MEMORY_GROWTH")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}  --embed-file ../Resources --embed-file ../external/lygia")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS} --embed-file ../Resources --embed-file ../external/lygia")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
set(CMAKE_EXECUTABLE_SUFFIX .html)
else()
    find_package(SDL2 REQUIRED)
    find_package(Freetype REQUIRED)
endif()

include_directories(SYSTEM external/stbimage)


file(GLOB HDR
     "include/*.h"
     "include/Utils/*.h"
)
file(GLOB SRC
     "src/*.cpp"
     "src/Utils/*.cpp"
)

include_directories(${CMAKE_SOURCE_DIR}/include  ${SDL2_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} )
include_directories(SYSTEM external/magic_enum)
add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME} PRIVATE ${SRC})
target_link_libraries(${PROJECT_NAME} glm GLESv2 ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})
target_include_directories(${PROJECT_NAME}
    PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Utils>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(BUILD_EXAMPLE)

    ###### IMGUI ###### 
  set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/)
  set(gui_hdrs  imgui.h backends/imgui_impl_sdl2.h backends/imgui_impl_opengl3.h )
  set(gui_objs imgui.cpp backends/imgui_impl_sdl2.cpp backends/imgui_impl_opengl3.cpp 
              imgui_demo.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp) 
  list(TRANSFORM gui_hdrs PREPEND ${IMGUI_DIR} )
  list(TRANSFORM gui_objs PREPEND ${IMGUI_DIR} )

include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends )




    add_executable(Example src/Example/main.cpp src/Example/Application.h src/Example/Application.cpp 
      src/Example/ShaderUI.h src/Example/ShaderUI.cpp ${gui_hdrs} ${gui_objs})
    target_link_libraries(Example  ${PROJECT_NAME} GLESv2 ) ## DO NOT FORGET GLESv2!!!!
endif()





