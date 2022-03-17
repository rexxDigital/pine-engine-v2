# Pine Engine
**A game engine written in C++**

### Current features (2021-11-12):
* Basic 3D renderer
* Basic 3D physics implementation via [ReactPhysics3D](https://www.reactphysics3d.com/) 
* Entities that are built up with components
* Editor built with [ImGui](https://github.com/ocornut/imgui)
* C++ "hot reloadable" .DLL for gameplay code
* And a lot more

### Plan:
* Get 3D rendering more stable and add more basic features
* Implement basic 2D support
* Implement multi threaded asset loading system

### Screenshot:
![Screenshot](https://i.imgur.com/qRzcv3e.png)
Small work in  progress "project", attempting to make a small horror game eventually.
Simple terrain mesh. A player entity with a camera and light attached to it. The player also has a player controller
script attached, providing simple FPS movement. The trees are being
generated by another script placing blueprints at random places.

Movement code:
```cpp
void PlayerController::OnRender( )
{
    const float speed = 0.05f;

    auto transform = m_Parent->GetTransform();
    auto camTransform = m_CameraEntity->GetTransform();

    // Handle rotation, however rotate only the camera for the pitch axis
    camTransform->Rotation.x += m_Pitch->Value();
    transform->Rotation.y += m_Yaw->Value();

    // Handle movement
    transform->Position += transform->GetForward() * speed * m_Forward->Value();
    transform->Position += transform->GetRight() * speed * m_Sideways->Value();
}
```

### Used libraries:
* GLFW (Window management)
* GLEW (OpenGL API)
* assimp (Open Asset Import Library)
* glm (OpenGL Mathematics)
* json by nlohmann (JSON parsing library)
* stb_image (Part of the single-file public domain for loading images)
* ImGui (Engine GUI library)
* Freetype (Font renderer)
* ReactPhysics3D (3D physics library)

