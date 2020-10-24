#pragma once

#include <ImGui/imgui.h>

namespace Editor::Gui::Windows {

	inline bool ShowEntitylist = true;
	inline bool ShowLevelViewport = true;
	inline bool ShowGameViewport = true;
	inline bool ShowAssetBrowser = true;
	inline bool ShowProperties = true;

	// Debug
	inline bool ShowRenderingContext = true;

	void RenderEntitylist( );

	void RenderViewports( );

	void RenderAssetBrowser( );

	void RenderProperties( );

	void RenderDebugWindows( );

	void RenderProjectWizard( );

}

