#pragma once

namespace Pine
{
    class FrameBuffer;
}

namespace Editor::Gui::Utility::EntityPickSystem
{

    void Setup();
    void Dispose();
    void Render();

    void SetViewportPosition(float x, float y);
    void SetViewportSize(float x, float y);

    Pine::FrameBuffer* GetFrameBuffer();

}
