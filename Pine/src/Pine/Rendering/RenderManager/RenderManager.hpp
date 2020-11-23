#pragma once
#include "..\RenderingContext\RenderingContext.hpp"

namespace Pine {
	class Camera;

	typedef void ( *RenderCallback )( );
}

namespace Pine::RenderManager {

	void SetRenderingContext( RenderingContext* renderingContext );
	RenderingContext* GetRenderingContext( );

	void SetPreRenderingCallback( RenderCallback fn );
	void SetPostRenderingCallback( RenderCallback fn );

	void Setup();
	void Run( );

}