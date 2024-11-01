cmake_minimum_required(VERSION 3.15)


project(renderer)
option(BUILD_EXAMPLE OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

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

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
set(USE_FLAGS "-s USE_SDL=2 -s FULL_ES3=1 -s OFFSCREEN_FRAMEBUFFER=1")
set(USE_FLAGS "${USE_FLAGS} -s MIN_WEBGL_VERSION=1 -s MAX_WEBGL_VERSION=2")
set(USE_FLAGS "${USE_FLAGS} -sASSERTIONS -sALLOW_MEMORY_GROWTH")
set(USE_FLAGS "${USE_FLAGS}  -s EXPORTED_FUNCTIONS=\"['_run_lua', '_main']\" -s EXPORTED_RUNTIME_METHODS=\"['ccall', 'cwrap']\"")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}  --embed-file ../Resources --embed-file ../external/lygia")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS} --embed-file ../Resources --embed-file ../external/lygia")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
set(CMAKE_EXECUTABLE_SUFFIX .html)
else()
    find_package(SDL2 REQUIRED)
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
target_link_libraries(${PROJECT_NAME} glm GLESv2 ${SDL2_LIBRARIES} freetype)
target_include_directories(${PROJECT_NAME}
    PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Utils>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Gui>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Gui/backends>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Gui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Gui/backends
)

if(BUILD_EXAMPLE)

  file(GLOB EXAMPLE_SRC
      "src/Example/*.cpp"
      "src/Example/*.h"
  )

  add_executable(Example ${EXAMPLE_SRC})
  target_link_libraries(Example  ${PROJECT_NAME} GLESv2 ) ## DO NOT FORGET GLESv2!!!!
endif()





