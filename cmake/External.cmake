include(FetchContent)

############# glm #############
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG master
)
FetchContent_MakeAvailable(glm)


############# SDL2 #############
FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL
  GIT_TAG SDL2
  )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSDL_TEST_LIBRARY=OFF -DSDL2_DISABLE_INSTALL=ON")
FetchContent_MakeAvailable(SDL2)
  

# include(ExternalProject)
# Fetch WavPack sources
# set(WAVPACK_INSTALL_DIR ${CMAKE_BINARY_DIR}/wavpack-install)

# ExternalProject_Add(wavpack
#   GIT_REPOSITORY https://github.com/dbry/WavPack.git
#   GIT_TAG 5.7.0
#   CONFIGURE_COMMAND ./autogen.sh && ./configure --prefix=${WAVPACK_INSTALL_DIR}
#   BUILD_COMMAND  make 
#   INSTALL_COMMAND make install
#   UPDATE_DISCONNECTED 1
#   BUILD_ALWAYS 1
# )
# add_custom_target(build_wavpack ALL  DEPENDS wavpack )

# Tell SDL_mixer where to find WavPack
# set(wavpack_INCLUDE_PATH ${WAVPACK_INSTALL_DIR}/include/wavpack CACHE PATH "")
# set(wavpack_LIBRARY ${WAVPACK_INSTALL_DIR}/lib/libwavpack.so CACHE FILEPATH "")
# message("\n\nPENIS\n\n${WAVPACK_INCLUDE_PATH}")

############# SDL_mixer #############
# -------------------------------
# SDL_mixer (disable optional deps)
# -------------------------------
set(SDL2MIXER_OPUS OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_FLAC OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_MOD OFF CACHE BOOL "" FORCE)     # disables libxmp
set(SDL2MIXER_MIDI OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_WAVPACK OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_MP3 ON  CACHE BOOL "" FORCE)     # keep MP3 if wanted
set(SDL2MIXER_OGG ON  CACHE BOOL "" FORCE)     # keep OGG if wanted
set(SDL2MIXER_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_DEPS_SHARED OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SDL2MIXER_VENDORED ON CACHE BOOL "" FORCE)


# Fetch SDL_mixer
FetchContent_Declare(
    SDL_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG        release-2.8.0  
    GIT_SHALLOW    TRUE
)
# Set SDL_mixer options before making it available
FetchContent_MakeAvailable(SDL_mixer)

# Get the source directory for SDL_mixer to access headers
FetchContent_GetProperties(SDL_mixer)
if(NOT sdl_mixer_POPULATED)
FetchContent_Populate(SDL_mixer)
endif()

# Debug message for SDL_mixer paths
if(TARGET SDL2_mixer-static)
    set(SDL_MIXER_TARGET SDL2_mixer-static)
    message(STATUS "Using SDL_mixer target: SDL2_mixer-static")
elseif(TARGET SDL2_mixer)
    set(SDL_MIXER_TARGET SDL2_mixer)
    message(STATUS "Using SDL_mixer target: SDL2_mixer")
elseif(TARGET SDL2_mixer::SDL2_mixer)
    set(SDL_MIXER_TARGET SDL2_mixer::SDL2_mixer)
    message(STATUS "Using SDL_mixer target: SDL2_mixer::SDL2_mixer")
elseif(TARGET SDL2_mixer::SDL2_mixer-static)
    set(SDL_MIXER_TARGET SDL2_mixer::SDL2_mixer-static)
    message(STATUS "Using SDL_mixer target: SDL2_mixer::SDL2_mixer-static")
else()
    message(FATAL_ERROR "Could not find SDL_mixer target!")
endif()
message(STATUS "SDL_mixer includes: ${SDL_MIXER_INCLUDES}")
message(STATUS "SDL_mixer source directory: ${sdl_mixer_SOURCE_DIR}")
message(STATUS "SDL_mixer binary directory: ${sdl_mixer_BINARY_DIR}")

############# glad #############
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
    glad_add_library(glad STATIC  API gl:core=4.6)
endif()



# if(BUILD_TESTS)
  # Google Test
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        main
  )
  FetchContent_MakeAvailable(googletest)
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# endif()