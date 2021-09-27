#pragma once
#include "..\RenderingContext\RenderingContext.hpp"

namespace Pine {
	class Camera;

	typedef void ( *RenderCallback )( int pass );
}

namespace Pine::RenderManager {

	void SetRenderingContext( RenderingContext* renderingContext );
	RenderingContext* GetRenderingContext( );

	void SetRenderingCallback( RenderCallback fn );

	void PrepareSceneRendering(  );
	void FinishSceneRendering(  );
	
	void Setup( );
	void Render( );

}