#pragma once

/*
	Pine - Game Engine
	Main header file
*/

// Core headers
#include "Core/Log/Log.hpp"
#include "Core/Window/Window.hpp"

// Common headers
#include "Assets/Assets.hpp"
#include "OpenGL/VertexArray/VertexArray.hpp"
#include "OpenGL/ShaderProgram/ShaderProgram.hpp"
#include "OpenGL/FrameBuffer/FrameBuffer.hpp"
#include "Renderer3D/Renderer3D.hpp"

typedef void (*RenderCallback)();

namespace Pine 
{
	// Main engine setup
	bool Setup();

	// Will enter the rendering loop
	void Run();

	// Clean up resources
	void Terminate();

	// Setup a rendering function, that the engine will call each frame for rendering
	void SetRenderingCallback(RenderCallback fn);

	// Set/get the target frame buffer.
	void SetFrameBuffer(FrameBuffer* framebuffer);
	FrameBuffer* GetFrameBuffer();
}
