#pragma once
#include "Pine/Components/NativeScript/NativeScript.hpp"
#include "Pine/Input/Input.hpp"

class PlayerController : public Pine::NativeScript
{
private:

	// Inputs:
	Pine::Input::InputBinding* m_LookPitch = nullptr;
	Pine::Input::InputBinding* m_LookYaw = nullptr;

public:

	void OnSetup( ) override;
	void OnUpdate( float deltaTime ) override;
	void OnRender( ) override;

};
