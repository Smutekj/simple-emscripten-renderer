
if(MSVC)
add_compile_options($<$<CONFIG:Release>:/O2>)
add_compile_options($<$<CONFIG:Debug>:/Od>)
else()
add_compile_options($<$<CONFIG:Release>:-O2>)
add_compile_options($<$<CONFIG:Debug>:-Og>)
endif()


if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(USE_FLAGS "-s USE_SDL=2 -s FULL_ES3=1 -s OFFSCREEN_FRAMEBUFFER=1")
    set(USE_FLAGS "${USE_FLAGS} -s MIN_WEBGL_VERSION=1 -s MAX_WEBGL_VERSION=2")
    set(USE_FLAGS "${USE_FLAGS} -s ASSERTIONS=1 -s ALLOW_MEMORY_GROWTH=1 -s ASYNCIFY=1 -s EXCEPTION_CATCHING_ALLOWED=yes")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 --profiling" )
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXECUTABLE_SUFFIX .html)
else()
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
        add_compile_options($<$<CONFIG:Release>:-march=native>)
    endif()
endif()


