#pragma once
#include <Pine/Components/NativeScript/NativeScript.hpp>

#include "Pine/Assets/Blueprint/Blueprint.hpp"
#include <glm/vec2.hpp>

class GameController : public Pine::NativeScript
{
private:
public:

	void OnSetup( ) override;
	void OnRender( ) override;
	void OnUpdate( float deltaTime ) override;

};
