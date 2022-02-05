#!/bin/sh
mkdir build_win32
cd build_win32
rm -rf *
cmake -DCMAKE_TOOLCHAIN_FILE=../mingw-w64_toolchain.cmake ..
cmake --build .
cd ..
cp build_win32/filewatch.exe bin
mkdir build_x64
cd build_x64
rm -rf *
cmake ..
cmake --build .
cd ..
cp build_x64/filewatch bin/filewatch.x64
