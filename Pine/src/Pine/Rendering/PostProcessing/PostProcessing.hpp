#pragma once

namespace Pine
{
	class FrameBuffer;
}

namespace Pine::PostProcessing
{

	void Setup( );
	void Dispose( );

	void Render( );
	
	Pine::FrameBuffer* GetRenderBuffer( );
	
}
