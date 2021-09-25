#pragma once

namespace Pine {
	class FrameBuffer;
}

namespace Editor::RenderingHandler
{

	void Setup( );

	Pine::FrameBuffer* GetFrameBuffer( );

	void SetViewportSize( int w, int h );

}
