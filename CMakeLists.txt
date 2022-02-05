cmake_minimum_required(VERSION 3.12)
project(filewatcherator)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_SHARED_LIBS off)
add_subdirectory(extern/efsw/)

add_executable(filewatch
    src/main.cpp
)

target_include_directories(filewatch
    PUBLIC extern/efsw/include
)

target_compile_options(filewatch PRIVATE -fexceptions -Os )

IF (WIN32)
    target_compile_definitions(filewatch PUBLIC _WIN32_WINNT=1281)
    target_link_libraries(filewatch
        efsw
        "${PROJECT_SOURCE_DIR}/extern/ws2_32.lib"
    )
ELSE()
    target_link_libraries(filewatch efsw)
ENDIF()

