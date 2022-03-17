#cmake_minimum_required(VERSION 3.0)
#project(PineEngine)

include_directories(${CMAKE_SOURCE_DIR}/Libraries/include "src/ImGui")

set(PROJECT_LINK_LIBS glfw3.lib glew32s.lib opengl32.lib assimp.lib freetype.lib reactphysics3d.lib)
link_directories(${CMAKE_SOURCE_DIR}/Libraries/binary)

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /MP /W0")
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

add_compile_definitions(GLEW_STATIC)

add_library(PineEngine STATIC ${SOURCES})