cmake_minimum_required(VERSION 3.15)


project(renderer)
option(BUILD_EXAMPLE OFF)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE) 
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

add_subdirectory(external/freetype)

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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSDL_TEST_LIBRARY=OFF -DSDL2_DISABLE_INSTALL=ON")
FetchContent_MakeAvailable(SDL2)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad
    GIT_TAG        v2.0.6
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    message("Fetching glad")
    FetchContent_MakeAvailable(glad)

    add_subdirectory("${glad_SOURCE_DIR}/cmake" glad_cmake)
    glad_add_library(glad STATIC  API gl:compatibility=4.6)
    include_directories(SYSTEM ${GLAD_DIR}/include)
endif()


if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
set(USE_FLAGS "-s USE_SDL=2 -s FULL_ES3=1 -s OFFSCREEN_FRAMEBUFFER=1")
set(USE_FLAGS "${USE_FLAGS} -s MIN_WEBGL_VERSION=1 -s MAX_WEBGL_VERSION=2")
set(USE_FLAGS "${USE_FLAGS} -s ASSERTIONS=0 -s ALLOW_MEMORY_GROWTH=1")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS} --profiling --embed-file ${CMAKE_SOURCE_DIR}/Resources --embed-file ${CMAKE_SOURCE_DIR}/external/lygia")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
set(CMAKE_EXECUTABLE_SUFFIX .html)
else()
endif()
include_directories(SYSTEM external/stbimage)

file(GLOB HDR
     "include/*.h"
     "include/Utils/*.h"
     "include/Gui/*.h"
     "include/Gui/backends/*.h"
)
file(GLOB SRC
     "src/*.cpp"
     "src/Utils/*.cpp"
     "src/Gui/*.cpp"
     "src/Gui/backends/*.cpp"
)

include_directories(${CMAKE_SOURCE_DIR}/include  ${SDL2_INCLUDE_DIRS})
include_directories(SYSTEM external/magic_enum)
include_directories(SYSTEM external/freetype)

add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME} PRIVATE ${SRC})
target_link_libraries(${PROJECT_NAME} PUBLIC SDL2 glm freetype glad)
target_include_directories(${PROJECT_NAME}
    PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Utils>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Gui>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Gui/backends>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
    SDL2
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Gui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Gui/backends
)

if(BUILD_EXAMPLES)
add_subdirectory(Examples)
endif()





