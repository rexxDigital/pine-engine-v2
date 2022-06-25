#pragma once
#include <Pine/Components/NativeScript/NativeScript.hpp>
#include <Pine/Assets/Blueprint/Blueprint.hpp>

#include <glm/vec2.hpp>
#include "Pine/Components/Light/Light.hpp"
#include "ImGui/imgui.h"

class MainMenuController : public Pine::NativeScript
{
private:

    Pine::Entity* m_Chandiler = nullptr;
    Pine::Entity* m_Light = nullptr;

    float m_LightTimer = 0.f;
    bool m_LightDirection = false;

    void UpdateLight(float deltaTime);
    void UpdateChandiler(float deltaTime);

    bool m_ChandilerDirection = false;
    float m_ChandilerRotation = 0.f;

    ImFont* m_MenuFont = nullptr;

public:

	void OnSetup( ) override;
	void OnRender( ) override;
	void OnRenderUI( ) override;
	void OnUpdate( float deltaTime ) override;

};
