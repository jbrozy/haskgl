cmake_minimum_required(VERSION 3.14.0)
project(
    haskgl
    VERSION 1.0
    LANGUAGES CXX
)

file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CXX_FLAGS "-Wall -stdlib=libc++")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(haskgl lexer.cpp  haskgl.cpp parser.cpp)
