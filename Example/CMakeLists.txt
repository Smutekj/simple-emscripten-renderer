project(Example)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT )

set(SRC_FILES 
	${CMAKE_CURRENT_LIST_DIR}/MyExample.cpp
	#${CMAKE_CURRENT_LIST_DIR}/../external/include/DynamicGles.h
	#${CMAKE_CURRENT_LIST_DIR}/../external/include/DynamicEgl.h
)

include_directories(SYSTEM "../external/freetype/include")
include_directories(SYSTEM "../external/magic_enum")
include_directories(SYSTEM "../external/stbimage")
include_directories(SYSTEM "../include")

#find_package(PVRShell REQUIRED MODULE)

# Apply SDK specific compile and linker options
if(WIN32) 
    add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES})
elseif(UNIX)
    add_executable(${PROJECT_NAME} ${SRC_FILES})
endif()
set_property(TARGET Example PROPERTY CXX_STANDARD 20)
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/bin/${CMAKE_BUILD_TYPE})

set_target_properties( Example
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)


if(WIN32)
    set(SHARED_LIB_SUFFIX ".dll")
elseif(UNIX)
    set(SHARED_LIB_SUFFIX ".so")
endif()

set(TARGET_OS_ARCH "${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}")

##### COPY SHARED OBJECTS OR DLLS
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to TARGETT
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/../external/lib/${TARGET_OS_ARCH}/libEGL${SHARED_LIB_SUFFIX}"      # <--this is in-file
        "${PROJECT_SOURCE_DIR}/../external/lib/${TARGET_OS_ARCH}/libGLESv2${SHARED_LIB_SUFFIX}"      # <--this is in-file
        "${CMAKE_BINARY_DIR}/" # <-- output directory
    )                 

target_link_libraries(Example PUBLIC
	Renderer
	PVRShell
)



