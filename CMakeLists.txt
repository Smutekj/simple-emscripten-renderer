cmake_minimum_required(VERSION 3.20)
project(Renderer)

include(FetchContent)

set(PVR_BUILD_EXAMPLES OFF)
set(PVR_PREBUILT_DEPENDENCIES OFF)

if(ANDROID)

elseif(APPLE)
	if (IOS)
		find_library(lib-gles OpenGLES)
		list(APPEND Renderer_LINK_LIBS ${lib-gles})
	endif()
elseif(UNIX)
	
	if(NOT PVR_WINDOW_SYSTEM)
		set(PVR_WINDOW_SYSTEM X11)
	endif()

	if(PVR_WINDOW_SYSTEM AND ("${PVR_WINDOW_SYSTEM}" STREQUAL Wayland))
		list(APPEND Renderer_LINK_LIBS ${WAYLAND_EGL_LIBRARIES})
		list(APPEND Renderer_INCLUDE_DIRECTORIES ${WAYLAND_EGL_INCLUDE_DIR})
	endif()
endif()



if(NOT PVR_PREBUILD_DEPENDENCIES)
	
	FetchContent_Declare(
	  freetype
	  GIT_REPOSITORY https://github.com/freetype/freetype.git
	  GIT_TAG        master
	)
	FetchContent_MakeAvailable(freetype)
	
	add_subdirectory(external/powervr-sdk)
endif()

#find_package(PVRShell REQUIRED MODULE)
# add_subdirectory(external/freetype)
	
###########################################

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT )

set(SRC_FILES 
	${CMAKE_CURRENT_LIST_DIR}/external/powervr-sdk/include/DynamicGles.h
	${CMAKE_CURRENT_LIST_DIR}/external/powervr-sdk/include/DynamicEgl.h
)

file(GLOB MY_SRC_FILES
		${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
		${CMAKE_CURRENT_LIST_DIR}/src/Utils/*.cpp 
		${CMAKE_CURRENT_LIST_DIR}/src/Gui/*.cpp )
file(GLOB MY_HDR_FILES 
		${CMAKE_CURRENT_LIST_DIR}/include/*.h
		${CMAKE_CURRENT_LIST_DIR}/include/Utils/*.h 
		${CMAKE_CURRENT_LIST_DIR}/include/Gui/*.h ) 
	
#add_platform_specific_executable(Renderer ${SRC_FILES} ${MY_SRC_FILES} ${MY_HDR_FILES} )
add_library(${PROJECT_NAME} STATIC ${MY_HDR_FILES} ${MY_SRC_FILES} ${SRC_FILES})

include_directories(SYSTEM ${FREETYPE_INCLUDE_DIRECTORIES}) #external/freetype/include)
include_directories(SYSTEM external/magic_enum)
include_directories(SYSTEM external/stbimage)

target_include_directories(${PROJECT_NAME}
    PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/external/powervr-sdk/include
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Utils>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Gui>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Gui/backends>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Gui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Gui/backends
	${Renderer_INCLUDE_DIRECTORIES}
)


# Apply SDK specific compile and linker options
include(cmake/utilities/compile_options.cmake)
apply_example_compile_options_to_target(Renderer)

set_property(TARGET Renderer PROPERTY CXX_STANDARD 20)
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/bin/${CMAKE_BUILD_TYPE})

set_target_properties( Renderer
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_link_libraries(Renderer PUBLIC
	PVRShell
	freetype
	${Renderer_LINK_LIBS}
)

option(BUILD_EXAMPLE ON)
if(BUILD_EXAMPLE)
	add_subdirectory(Example)
endif()


