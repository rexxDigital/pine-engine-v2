cmake_minimum_required(VERSION 3.0)
project(GameHost)

include_directories({$LibIncludePath} "src/" "${CMAKE_SOURCE_DIR}/Pine/src" "${CMAKE_SOURCE_DIR}/Libraries/glm/include" "${CMAKE_SOURCE_DIR}/Libraries/reactphysics3d/include" "${CMAKE_SOURCE_DIR}/Libraries/json/single_include/nlohmann" "${CMAKE_SOURCE_DIR}/Libraries/stb")

#include_directories("../Libraries/include" "src/" "../Pine/src")

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -w")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

add_executable(GameHost ${SOURCES})

target_link_libraries(GameHost PineEngine)