#pragma once
#include <Pine/Components/NativeScript/NativeScript.hpp>
#include <Pine/Input/Input.hpp>

#include "Pine/Assets/Blueprint/Blueprint.hpp"
#include <glm/vec2.hpp>

class PlayerController : public Pine::NativeScript
{
private:
    Pine::Entity* m_CameraEntity = nullptr;

    Pine::InputBinding* m_Pitch = nullptr;
    Pine::InputBinding* m_Yaw = nullptr;

    Pine::InputBinding* m_Forward = nullptr;
    Pine::InputBinding* m_Sideways = nullptr;
public:

    void OnSetup( ) override;
    void OnRender( ) override;
    void OnUpdate( float deltaTime ) override;

};
