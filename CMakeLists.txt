cmake_minimum_required(VERSION 3.15)


project(renderer)
option(BUILD_EXAMPLE OFF)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE) 
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

add_subdirectory(external/freetype)

include(cmake/External.cmake)
include(cmake/CompilerFlags.cmake)

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


add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME} PRIVATE ${SRC})
target_link_libraries(${PROJECT_NAME} PUBLIC SDL2 SDL2main glm freetype glad)
target_include_directories(${PROJECT_NAME}
    PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Utils>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Gui>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Gui/backends>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    SDL2
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Gui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Gui/backends
)

### This should be target_include_directories
include_directories(${CMAKE_SOURCE_DIR}/include  ${SDL2_INCLUDE_DIRS})
include_directories(SYSTEM external/stbimage)
include_directories(SYSTEM external/magic_enum)
include_directories(SYSTEM external/freetype)

if(BUILD_EXAMPLES)
add_subdirectory(Examples)
endif()


if(BUILD_TESTS)
    enable_testing()
    include(GoogleTest)

    message("\nADDING TESTS!!!\n")

    add_executable(test_all src/Tests/main.cpp)
    target_link_libraries(test_all PRIVATE gtest renderer)

    gtest_discover_tests(test_all)
endif()





