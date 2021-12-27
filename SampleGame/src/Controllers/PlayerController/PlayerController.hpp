#pragma once
#include <Pine/Components/IComponent/IComponent.hpp>
#include <Pine/Input/Input.hpp>

class PlayerController : public Pine::IComponent
{
private:

	Pine::InputBinding* m_Pitch;
	Pine::InputBinding* m_Yaw;

public:

	void OnSetup( ) override;
	void OnRender( ) override;
	void OnUpdate( float deltaTime ) override;

};