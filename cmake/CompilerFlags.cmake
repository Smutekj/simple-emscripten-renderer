function(set_target_compiler_flags target_name)
  if(MSVC)
    target_compile_options(${target_name} PRIVATE $<$<CONFIG:Release>:/O2>)
  else()
    target_compile_options(${target_name} PRIVATE $<$<CONFIG:Release>:-O2>)
  endif()
  
  
  if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    target_compile_options(${target_name} PUBLIC
    "-sUSE_SDL=2"
    )
    
    target_link_options(${target_name} PUBLIC
    "-s USE_SDL=2 -s FULL_ES3=1"
    "-s OFFSCREEN_FRAMEBUFFER=1"
    "-s MIN_WEBGL_VERSION=1"
    "-s MAX_WEBGL_VERSION=2"
    "-s ASSERTIONS=1"
    "-s ALLOW_MEMORY_GROWTH=1"
    "-s ASYNCIFY=1"
    "-s EXCEPTION_CATCHING_ALLOWED=yes" 
    )
  endif()
  
#   if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
#   set(USE_FLAGS "-s USE_SDL=2 -s FULL_ES3=1 -s OFFSCREEN_FRAMEBUFFER=1")
#   set(USE_FLAGS "${USE_FLAGS} -s MIN_WEBGL_VERSION=1 -s MAX_WEBGL_VERSION=2")
#   set(USE_FLAGS "${USE_FLAGS} -s ASSERTIONS=1 -s ALLOW_MEMORY_GROWTH=1 -s ASYNCIFY=1 -s EXCEPTION_CATCHING_ALLOWED=yes")
  
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 --profiling" )
#   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
#   else()
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
  target_compile_options(${target_name} PRIVATE $<$<CONFIG:Release>:-march=native>)
  endif()

  endfunction()

