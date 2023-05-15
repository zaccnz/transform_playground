# transform playground

try it out online at [zaccnz.github.io/transform_playground](https://zaccnz.github.io/transform_playground/)  

this is a tool intended to help me build intuition on how the opengl transformations work, and how we can combine these transformations to build a scene.  

started in 2022 semester 1 but not completed.  the original code used OpenGL immediate mode, freeglut, GLM and imgui.  however, I have decided to rewrite it using Raylib so that it can work cross platform.  

### technologies

- [Raylib](https://github.com/raysan5/raylib)  
- [rlImGui](https://github.com/raylib-extras/rlImGui)  
- [imgui](https://github.com/ocornut/imgui)  
- [json](https://github.com/nlohmann/json)  
- [fmt](https://github.com/fmtlib/fmt)  
- [nativefiledialog-extended](https://github.com/btzy/nativefiledialog-extended)  

### build instructions

```
cd transform_playground
mkdir build && cd build
cmake ..
```

### wasm build instructions

after running `source emsdk_env.sh` or `emsdk_env.bat` (in emsdk path) for Linux/macOS or Windows, respectively:

```
cd transform_playground
mkdir wasmbuild && cd wasmbuild
cmake .. -DCMAKE_TOOLCHAIN_FILE=[EMSCRIPTEN_SDK_PATH]/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DPLATFORM=Web
make
```