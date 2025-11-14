cmake_minimum_required (VERSION 3.17)

project (FileBench LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

add_executable (filebench filebench.cpp)
if (MSVC)
	target_compile_definitions (filebench PUBLIC "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE")
endif ()
