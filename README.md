A 2D batch renderer using OpenGL compatible with GLES 3 in order to make it possible to build into WebAssembly with [emscripten](https://emscripten.org/).

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
cmake .. -B . -DBUILD_EXAMPLES=ON
cmake --build . --config Release 
```
The build creates a static library in the corresponding RUNTIME_OUTPUT_DIRECTORY.

***CMake***

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


**Emscripten Build**

(I have not tried this on Windows, because I don't need it. But on Linux it should work)
To build with emscripten just activate the emscripten emsdk just as described in the emscripten [docs](https://emscripten.org/docs/getting_started/downloads.html).  

If successfull, you should be able to use the `emcmake` and `emmake` commands.
```
mkdir build-web
cd build-web
emcmake cmake .. -B . -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Release
emmake  make -j6
```
If everything worked correctly there should be .html, .js, and .wasm files in the build folder.

WARNING:
If you are using resources and building with emscripten, DO NOT FORGET to add `--embed-file "path to resources"` flags to the linker and compiler. 
See emscipten [docs](https://emscripten.org/docs/porting/files/packaging_files.html). (This warning is mostly for me :D)

