# Overview
GraphicsPipeline is a high-performance low-level graphics engine.  It is designed to be a light-weight abstraction layer over platform specific functions and graphics APIs to support the development larger, more complex, domain specific engines ontop of this one.  The main goals are high efficiency and broad platform support.

# Building
Building is handled by CMake.  It can be built using the following commands.
```bash
> mkdir build
> cd build
> cmake <source directory>
> cmake --build .
```

## Building for Android
Building for android requires the [Android NDK](https://developer.android.com/ndk) The NDK should provide a toolchain file that can be used to configure the cmake build.  A build directory can be configured with the following command:
```bash
> cmake \
    -DCMAKE_TOOLCHAIN_FILE=<NDK directory>/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=x86 \
    -DANDROID_NATIVE_API_LEVEL=28 \
    <source directory>
```

## Building for WebAssembly
Building requires the [emscripten](https://emscripten.org/) compiler.  A toolchain file is provided to configure the build.
```bash
> cmake -DCMAKE_TOOLCHAIN_FILE=<source directory>/cmake/Modules/Emscripten.cmake <source directory>
```

## Building Qt5 support
Qt5 support can be built alongside native interface support.  Qt5 support can be turned on with the cmake flag `GP_BUILD_QT5`.
```bash
> cmake -DGP_BUILD_QT5=ON <source directory>
```
