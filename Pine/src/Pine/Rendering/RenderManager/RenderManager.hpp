#pragma once
#include "../RenderingContext/RenderingContext.hpp"
#include "../../Core/Interfaces/Interfaces.hpp"
#include <vector>
#include <unordered_map>

namespace Pine
{
	class Camera;
    class Model;
    class ModelRenderer;

	enum class RenderStage
	{
		PreRender,
		PostRenderEntities,
		PostRender
	};

	typedef void ( *RenderCallback )( RenderStage pass );

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

        // Returns a map of all entities with a ModelRenderer that the engine would render,
        // useful if you for need to render entities yourself.
        virtual std::unordered_map<Model*, std::vector<ModelRenderer*>> GetRenderingBatch( ) = 0;

        
	};

}
