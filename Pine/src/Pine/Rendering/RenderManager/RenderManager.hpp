#pragma once
#include "..\RenderingContext\RenderingContext.hpp"
#include "../../Core/Interfaces/Interfaces.hpp"

namespace Pine
{
	class Camera;

	typedef void ( *RenderCallback )( int pass );

	class IRenderManager : public IInterface
	{
	public:

		virtual void SetRenderingContext( RenderingContext* renderingContext ) = 0;
		virtual RenderingContext* GetRenderingContext( ) = 0;

		// Called during different passes during the rendering,
		// 0 == pre render
		// 1 == after rendering scene (pre post-processing)
		// 2 == after all rendering is done
		virtual void SetRenderingCallback( RenderCallback fn ) = 0;

		virtual void PrepareSceneRendering( ) = 0;
		virtual void FinishSceneRendering( ) = 0;

		virtual void Render( ) = 0;

	};

}
