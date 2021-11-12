# Pine Engine
A game engine written in C++

Current features (2021-11-12):
* Basic 3D renderer
* Basic 3D physics implementation via [ReactPhysics3D](https://www.reactphysics3d.com/) 
* Entities that are built up with components
* Editor built with [ImGui](https://github.com/ocornut/imgui)
* C++ "hot reloadable" .DLL for gameplay code
* And a lot more

Plan:
* Get 3D rendering more stable and add more basic features
* Implement basic 2D support
* Implement multi threaded asset loading system

Screenshot:
![Screenshot](https://i.imgur.com/OcqGjev.png)

Used libraries:
* GLFW (Window management)
* GLEW (OpenGL API)
* assimp (Open Asset Import Library)
* glm (OpenGL Mathematics)
* json by nlohmann (JSON parsing library)
* angelscript (Engine scripting API)
* stb_image (Part of the single-file public domain for loading images)
* ImGui (Engine GUI library)
* Freetype (Font renderer)
* ReactPhysics3D (3D physics library)

