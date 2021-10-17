#pragma once
#include "..\RenderingContext\RenderingContext.hpp"

namespace Pine {
	class Camera;

	typedef void ( *RenderCallback )( int pass );
}

namespace Pine::RenderManager {

	void SetRenderingContext( RenderingContext* renderingContext );
	RenderingContext* GetRenderingContext( );

	// Called during different passes during the rendering,
	// 0 == pre render
	// 1 == after rendering scene (pre post-processing)
	// 2 == after all rendering is done
	void SetRenderingCallback( RenderCallback fn );

	void PrepareSceneRendering(  );
	void FinishSceneRendering(  );
	
	void Setup( );
	void Render( );

}