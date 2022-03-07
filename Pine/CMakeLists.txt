project(Pine)
cmake_minimum_required(VERSION 3.0)

include_directories("../Libraries/include" "src/ImGui")

set(PROJECT_LINK_LIBS glfw3.lib glew32s.lib opengl32.lib assimp.lib freetype.lib reactphysics3d.lib)
link_directories("../Libraries/binary")

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /MP /W0")
set(LIBRARY_OUTPUT_PATH "../../bin")

add_compile_definitions(GLEW_STATIC)

add_library(Pine STATIC ${SOURCES})