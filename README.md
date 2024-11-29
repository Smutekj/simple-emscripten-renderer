A 2D batch renderer using OpenGL ES 2 (and some features of ES 3) in order to make it possible to build into WebAssembly.

**Features**

  - Batch rendering of sprites and vertex arrays. Batching is done using a combination of: Shader x 2 Textures
  - Static and dynamic draw calls (static not really tested yet however)
  - Text rendering based on FreeType2 using character SDFs makes it easy to add effects like borders to text.
  - Can choose data format of textures, so it is possible to use HDR.
  - Framebuffers and offscreen rendering

   
The whole point of this was for me to learn how OpenGL, how to do post-effects and to make some projects for my portfolio.

**Build**
The build is done using CMake and should work on Linux and Windows (Possibly even on Mac, but I have no comfortable way of testing):

```
mkdir build
cd build
cmake .. -B . -DBUILD_EXAMPLE=ON
cmake --build . --config Release 
```
The build creates a static library in the corresponding RUNTIME_OUTPUT_DIRECTORY.

**CMake**
If you want to use it as a library in your project, the best way is to use FetchContent in CMake.
Add this into your CMakeLists.txt
```
include(FetchContent)
FetchContent_Declare(
  Renderer
  GIT_REPOSITORY  https://github.com/Smutekj/simple-emscripten-renderer
  GIT_TAG  master 
)
FetchContent_MakeAvailable(Renderer)
```

Lastly, do not forget to add:
```
target_link_libraries(YOUR_TARGET ...other deps... Renderer) 
```
into your projects CMakeLists.txt.
