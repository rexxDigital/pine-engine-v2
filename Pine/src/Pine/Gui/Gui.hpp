#pragma once

typedef void (*GuiRenderCallback)();

namespace Pine::Gui {

	void SetGuiRenderCallback(GuiRenderCallback callback);

	void Setup();
	void Dispose();
	void Render();

}