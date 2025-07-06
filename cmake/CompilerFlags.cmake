function(set_target_compiler_flags target_name)
  if(MSVC)
    target_compile_options(${target_name} PRIVATE $<$<CONFIG:Release>:/O2>)
  else()
    target_compile_options(${target_name} PRIVATE $<$<CONFIG:Release>:-O2>)
  endif()
  
  
  if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    target_compile_options(${target_name} PUBLIC
    "-sUSE_SDL=2"
    "-sMIN_WEBGL_VERSION=2"
    "-sMAX_WEBGL_VERSION=2"
    )
    
    target_link_options(${target_name} PUBLIC
    "-sUSE_SDL=2"
    "-sFULL_ES3=1"
    "-sUSE_WEBGL2=1"
    "-sOFFSCREEN_FRAMEBUFFER=1"
    "-sMIN_WEBGL_VERSION=2"
    "-sMAX_WEBGL_VERSION=2"
    "-sASSERTIONS=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sASYNCIFY=1"
    "-sEXCEPTION_CATCHING_ALLOWED=yes" 
    )
  endif()

  if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
  target_compile_options(${target_name} PRIVATE $<$<CONFIG:Release>:-march=native>)
  endif()

  endfunction()

