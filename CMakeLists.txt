cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Отключение нестандартных расширений компилятора

project(laba3)

include_directories (include)
add_executable(laba3 src/laba3.cpp src/source_lexeme.cpp include/header.hpp include/poly.hpp)