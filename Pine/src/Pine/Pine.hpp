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
	// Pointers to all interfaces used within the engine
	PineInstance* GetPineInstance( );

	// Time since application was started
	float GetTime( );

	// Performance counters
	float GetFrameTime( );
	float GetUpdateTime( );
	int GetFPS( );

	// Allow/disallow OnUpdate being called on components, primarily used by the editor extension.
	void SetAllowUpdates( bool value );
	bool IsAllowingUpdates( );

	// Main engine setup
	bool Setup( );

	// Will enter the rendering loop
	void Run( );

	// Clean up resources
	void Terminate( );

}
