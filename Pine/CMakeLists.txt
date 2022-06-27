cmake_minimum_required(VERSION 3.0)
project(PineEngine)

include_directories({$LibIncludePath} "src/ImGui" "${CMAKE_SOURCE_DIR}/Libraries/glm/include" "${CMAKE_SOURCE_DIR}/Libraries/reactphysics3d/include" "${CMAKE_SOURCE_DIR}/Libraries/json/single_include/nlohmann" "${CMAKE_SOURCE_DIR}/Libraries/stb")
#set(PROJECT_LINK_LIBS glfw3.lib glew32s.lib opengl32.lib assimp.lib freetype.lib reactphysics3d.lib)
#link_directories(${CMAKE_SOURCE_DIR}/Libraries/binary)

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -w -fPIC")
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

add_compile_definitions(GLEW_STATIC)

add_library(PineEngine STATIC ${SOURCES} src/Pine/Rendering/ShadowManager/ShadowManager.hpp src/Pine/Rendering/ShadowManager/ShadowManager.cpp)

target_link_libraries(PineEngine glfw GL GLEW assimp reactphysics3d)