#pragma once

/*
	Pine - Game Engine
*/

// Core headers
#include "Core/Interfaces/Interfaces.hpp"
#include "Core/Log/Log.hpp"
#include "Core/Window/Window.hpp"

// Common headers
#include "Assets/Assets.hpp"
#include "OpenGL/VertexArray/VertexArray.hpp"
#include "OpenGL/ShaderProgram/ShaderProgram.hpp"
#include "OpenGL/FrameBuffer/FrameBuffer.hpp"
#include "Rendering/Renderer3D/Renderer3D.hpp"

namespace Pine
{

	void SetAllowUpdates( bool value );
	bool IsAllowingUpdates( );

	float GetTime( );
	PineInstance* GetPineInstance( );

	// Main engine setup
	bool Setup( );

	// Will enter the rendering loop
	void Run( );

	// Clean up resources
	void Terminate( );

}
