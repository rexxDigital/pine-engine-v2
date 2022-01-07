#pragma once
#include <Pine/Components/NativeScript/NativeScript.hpp>
#include "Pine/Input/Input.hpp"

class InstancedRenderingTest : public Pine::NativeScript
{
private:
public:  

	void OnSetup( ) override;
	void OnUpdate( float deltaTime ) override;

};
