project(glad)


if(ANDROID)
    add_library(glad STATIC glad-gles3-ext/src/glad.c)
    target_include_directories(glad PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glad-gles3-ext/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
else()
    add_library(glad STATIC glad-gl4/src/glad.c)
    target_include_directories(glad PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glad-gl4/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
endif()
