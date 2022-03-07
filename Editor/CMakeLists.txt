project(PineEditor)
cmake_minimum_required(VERSION 3.0)

include_directories("../Libraries/include" "src/" "../Pine/src")

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /MP /W0")
set(EXECUTABLE_OUTPUT_PATH "../../bin")

add_executable(PineEditor ${SOURCES})

target_link_libraries(PineEditor "../../bin/Debug/Pine" "../../Libraries/binary/glfw3" "../../Libraries/binary/glew32s" "opengl32" "../../Libraries/binary/assimp" "../../Libraries/binary/freetype" "../../Libraries/binary/reactphysics3d-debug")
