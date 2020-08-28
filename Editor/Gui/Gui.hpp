#pragma once
#include "Pine/OpenGL/FrameBuffer/FrameBuffer.hpp"

namespace Pine { class Entity; }

namespace Editor::Gui {

	Pine::FrameBuffer* GetViewportFrameBuffer();

	Pine::Entity* GetSelectedEntity();
	void SetSelectedEntity(Pine::Entity* entity);

	void Setup();
	void Dispose();
	void Run();

}
