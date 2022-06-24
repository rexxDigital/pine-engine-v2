cmake_minimum_required(VERSION 3.0)
project(Pine)

# Libraries that require compiliation
add_subdirectory(Libraries/reactphysics3d)

# Setup additonal include
set(LibIncludePath "${LibIncludePath} ${CMAKE_SOURCE_DIR}/Libraries/json/single_include/nlohmann")
set(LibIncludePath "${LibIncludePath} ${CMAKE_SOURCE_DIR}/Libraries/reactphysics3d/include")
set(LibIncludePath "${LibIncludePath} ${CMAKE_SOURCE_DIR}/Libraries/stb")
set(LibIncludePath "${LibIncludePath} ${CMAKE_SOURCE_DIR}/Libraries/glm/include")

add_subdirectory(Pine)
add_subdirectory(Editor)
add_subdirectory(GameHost)