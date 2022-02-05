# CMake Toolchain File for MinGW-w64 (64-bit) Cross Compilation

# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

# which tools to use
set(CMAKE_C_COMPILER   i686-w64-mingw32-gcc-posix)
set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++-posix)

# here is where the target environment located
set(CMAKE_FIND_ROOT_PATH  /usr/i686-w64-mingw32/)

# adjust the default behavior of the FIND_XXX() commands:

# search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_INSTALL_PREFIX ${CMAKE_FIND_ROOT_PATH}/usr CACHE FILEPATH
    "install path prefix")

include(GNUInstallDirs)
# initialize required linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "-s -Os -static-libgcc -static-libstdc++ -lsetupapi -lwsock32 ")


# end of toolchain file