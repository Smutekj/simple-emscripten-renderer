cmake_minimum_required(VERSION 3.15)
include(GNUInstallDirs)

project(renderer)
option(BUILD_EXAMPLES ON)
option(BUILD_TESTS ON)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 20)

include(cmake/CompilerFlags.cmake)
include(cmake/CompilerWarnings.cmake)

add_subdirectory(external/freetype)
include(cmake/External.cmake)

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

set(TARGET_LIBRARY_NAME ${PROJECT_NAME})
add_library(${TARGET_LIBRARY_NAME} STATIC)
target_sources(${TARGET_LIBRARY_NAME} PRIVATE ${SRC})
target_link_libraries(${TARGET_LIBRARY_NAME} PUBLIC SDL2 glm freetype glad )
set_target_properties(${TARGET_LIBRARY_NAME} PROPERTIES CXX_STANDARD 20)
set_target_properties(${TARGET_LIBRARY_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set_target_properties(${TARGET_LIBRARY_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set_project_warnings(${TARGET_LIBRARY_NAME})

target_include_directories(${TARGET_LIBRARY_NAME}
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

if(BUILD_EXAMPLES)
message("\n!ADDING EXAMPLES!")
add_subdirectory(Examples)
endif()

if(BUILD_TESTS)
    enable_testing()
    include(GoogleTest)

    message("\n!ADDING TESTS!\n")
    add_executable(test_all2 src/Tests/main.cpp src/Tests/test_context.cc src/Tests/test_shader.cc)
    target_link_libraries(test_all2 PUBLIC GTest::gtest_main ${TARGET_LIBRARY_NAME})
    set_target_properties(test_all2 PROPERTIES CXX_STANDARD 20)
    ### output test executable is right now in major bin directory because there is just one exe
    # set_target_properties(test_all2 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/tests) 

    gtest_discover_tests(test_all2
        DISCOVERY_MODE PRE_TEST
    )

    ### this is not working :(
    ### see: https://discourse.cmake.org/t/msvs-gtest-discover-tests-environment/9225/3
    #if(WIN32)
    #    set_target_properties(test_all2 PROPERTIES ENVIRONMENT
    #    "PATH=$<SHELL_PATH:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}>$<SEMICOLON>$ENV{PATH}"
    #    )
    #endif()

endif()
