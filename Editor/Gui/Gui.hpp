#pragma once
#include "Pine/Assets/IAsset/IAsset.hpp"
#include "Pine/OpenGL/FrameBuffer/FrameBuffer.hpp"

namespace Pine { class Entity; }

namespace Editor::Gui {

	Pine::FrameBuffer* GetViewportFrameBuffer();

	Pine::Entity* GetSelectedEntity();
	void SetSelectedEntity(Pine::Entity* entity);

	Pine::IAsset* GetSelectedAsset( );
	void SetSelectedAsset( Pine::IAsset* asset );

	void Setup();
	void Dispose();
	void Run();

}
