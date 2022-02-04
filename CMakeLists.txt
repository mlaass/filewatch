cmake_minimum_required(VERSION 3.12)
project(filewatcherator)

set(CMAKE_CXX_STANDARD 17)



add_subdirectory(extern/efsw/)


add_executable(filewatch
 src/main.cpp)

target_include_directories(filewatch
PUBLIC extern/efsw/include
)

target_compile_options(filewatch PRIVATE -fexceptions)

target_link_libraries(filewatch
efsw
)
