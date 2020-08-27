#pragma once
#include "Pine/OpenGL/FrameBuffer/FrameBuffer.hpp"

namespace Editor::Gui {

	Pine::FrameBuffer* GetViewportFrameBuffer();

	void Setup();
	void Dispose();
	void Run();

}
