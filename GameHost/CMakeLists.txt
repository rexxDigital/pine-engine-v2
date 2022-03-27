cmake_minimum_required(VERSION 3.0)

include_directories("../Libraries/include" "src/" "../Pine/src")

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /MP /W0")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

add_executable(GameHost ${SOURCES})

target_link_libraries(GameHost ${CMAKE_SOURCE_DIR}/bin/PineEngine.lib ${CMAKE_SOURCE_DIR}/Libraries/binary/glfw3.lib ${CMAKE_SOURCE_DIR}/Libraries/binary/glew32s.lib "opengl32" ${CMAKE_SOURCE_DIR}/Libraries/binary/assimp.lib ${CMAKE_SOURCE_DIR}/Libraries/binary/freetype.lib ${CMAKE_SOURCE_DIR}/Libraries/binary/reactphysics3d-debug.lib)
