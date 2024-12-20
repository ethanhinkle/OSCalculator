#!/bin/bash
BUILD_PLATFORM="windows_mingw"
CMAKE_C_COMPILER="x86_64-w64-mingw32-gcc"
CMAKE_CXX_COMPILER="x86_64-w64-mingw32-g++"
CMAKE_BUILD_TYPE="Debug"

cmake -G "MinGW Makefiles" \
      -DBUILD_PLATFORM="$BUILD_PLATFORM" \
      -DCMAKE_C_COMPILER="$CMAKE_C_COMPILER" \
      -DCMAKE_CXX_COMPILER="$CMAKE_CXX_COMPILER" \
      -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" ..
cmake --build . -j4