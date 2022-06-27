#include "PostProcessing.hpp"

#include <random>

#include "../../OpenGL/FrameBuffer/FrameBuffer.hpp"

#include <GL/glew.h>
#include <glm/trigonometric.hpp>

#include "../../Assets/Shader/Shader.hpp"
#include "../../Core/Log/Log.hpp"
#include "../../OpenGL/VertexArray/VertexArray.hpp"
#include "../Renderer3D/Renderer3D.hpp"
#include "../RenderManager/RenderManager.hpp"

namespace Pine
{

	class CPostProcessing : public IPostProcessing
	{
	private:

		FrameBuffer* g_PostProcessingBuffer = nullptr;
		VertexArray* g_ScreenQuad = nullptr;
		Shader* g_PostProcessingShader = nullptr;

	public:

		void Setup( ) override
		{
			Log->Debug( "Pine::PostProcessing->Setup()" );

			// Setup rendering frame buffer
			g_PostProcessingBuffer = new Pine::FrameBuffer;
			g_PostProcessingBuffer->Create( 1920, 1080, Buffers::TextureBuffer | Buffers::DepthStencilBuffer );

			// Setup the quad vertex array
			g_ScreenQuad = new Pine::VertexArray;
			g_ScreenQuad->Create( );
			g_ScreenQuad->Bind( );

			const std::vector<float> quads = 
			{
				-1.f, 1.f, 0.f,
				-1.f, -1.f, 0.f,
				1.f, -1.f, 0.f,
				1.f, 1.f, 0.f,
			};

			const std::vector<int> indices = 
			{
				0,1,3,
				3,1,2
			};

			const std::vector<float> uvs = 
			{
				0,0,
				0,1,
				1,1,
				1,0
			};

			g_ScreenQuad->StoreFloatBuffer( quads, 3, 0 );
			g_ScreenQuad->StoreFloatBuffer( uvs, 2, 2 );
			g_ScreenQuad->StoreIntBuffer( indices );

			// Prepare post processing shader
			g_PostProcessingShader = Assets->GetAsset<Shader>( "Assets/Engine/Shaders/PostProcessing.shr" );
		}

		void Dispose( ) override
		{
			g_PostProcessingBuffer->Dispose( );
			g_ScreenQuad->Dispose( );

			delete g_ScreenQuad;
			delete g_PostProcessingBuffer;
		}

		void Render( ) override
		{
			const auto renderingContext = RenderManager->GetRenderingContext( );

			// At this point, the target frame buffer is already bound, so we don't need to worry about that.

			// Clear the buffers
			glClearColor( 1.f, 0.f, 0.f, 1.f );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			// Reset the viewport size.
			glViewport( 0, 0, renderingContext->m_Width, renderingContext->m_Height );

			// Disable depth test
			glDisable( GL_DEPTH_TEST );

			// Bind the vertex array and the shader program
			g_ScreenQuad->Bind( );
			g_PostProcessingShader->Use( );

			const auto cam = RenderManager->GetRenderingContext( )->m_Camera;

			if ( !cam )
				return;

			g_PostProcessingShader->GetUniformVariable( "viewport" )->LoadVector2( glm::vec2( renderingContext->m_Width / 1920.f, renderingContext->m_Height / 1080.f ) );

			// Bind frame buffer
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, g_PostProcessingBuffer->GetTextureId( ) );

			Renderer3D->UpdateTextureBound( 0, g_PostProcessingBuffer->GetTextureId( ) );

			// Just do a simple render call
			glDrawElements( GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr );
		}

		FrameBuffer* GetRenderBuffer( ) override
		{
			return g_PostProcessingBuffer;
		}

	};

	IPostProcessing* CreatePostProcessingInterface( )
	{
		return new CPostProcessing( );
	}

}

