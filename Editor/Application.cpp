#include <iostream>
#include <Pine/Pine.hpp>
#include <Pine/Gui/Gui.hpp>
#include "Gui/Gui.hpp"

void OnRender() {

}

void OnRenderGui() {

    Editor::Gui::Run();

}

int main()
{
    if (!Pine::Setup()) {
        return 1;
    }

    Pine::Window::SetSize(1280, 720);

    Editor::Gui::Setup();
	
	// Setup rendering callbacks
    Pine::SetRenderingCallback(OnRender);
    Pine::Gui::SetGuiRenderCallback(OnRenderGui);

    // Setup frame buffer.
    Pine::SetFrameBuffer(Editor::Gui::GetViewportFrameBuffer());
	
    Pine::Run();

    Editor::Gui::Dispose();
	
    Pine::Terminate();

    return 0;
}