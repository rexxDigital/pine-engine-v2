#pragma once
#include "..\RenderingContext\RenderingContext.hpp"

namespace Pine {
	class Camera;

	typedef void ( *RenderCallback )( );
}

namespace Pine::RenderManager {

	void SetRenderingContext( RenderingContext* renderingContext );
	RenderingContext* GetRenderingContext( );

	void SetRenderingCallback( RenderCallback fn );

	void Run( );

}