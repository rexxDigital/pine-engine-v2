#pragma once
#include <Pine/Components/NativeScript/NativeScript.hpp>

#include "Pine/Input/Input.hpp"

class RGBExperiment : public Pine::NativeScript
{
private:
	bool m_LightMovement = false;
	float m_MovementTime = 0.0f;

	Pine::Entity* m_R;
	Pine::Entity* m_G;
	Pine::Entity* m_B;
public:  

	void OnSetup( ) override;
	void OnUpdate( float deltaTime ) override;

};
