#pragma once
#include <Pine/Components/NativeScript/NativeScript.hpp>

class GameController : public Pine::NativeScript
{
private:
public:

	void OnSetup( ) override;
	void OnUpdate( float deltaTime ) override;

};