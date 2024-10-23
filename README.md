A 2D batch renderer using OpenGL ES 2 (and some features of ES 3) in order to make it possible to build into WebAssembly.

**Features**

  - Batch rendering of sprites and vertex arrays. Batching is done using a combination of: Shader x 2 Textures
  - Static and dynamic draw calls (static not really tested yet however)
  - Text rendering based on FreeType2 using character SDFs makes it easy to add effects like borders to text.
  - Can choose data format of textures, so it is possible to use HDR.
  - Framebuffers and offscreen rendering

   
The whole point of this was for me to learn how OpenGL, how to do post-effects and to make some projects for my portfolio.

**Build**

CMake:

if being used as a library add:
```

```
into your CMakeLists.txt
Do not forget to add the renderer into target_link_libraries(...);


```
mkdir build
cd build
cmake .. -B . -DBUILD_EXAMPLE=ON
make 
```


