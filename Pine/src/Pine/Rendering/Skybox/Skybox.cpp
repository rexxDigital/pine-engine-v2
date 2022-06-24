#include "Skybox.hpp"
#include "../../OpenGL/VertexArray/VertexArray.hpp"
#include "../../Assets/Shader/Shader.hpp"

#include <GL/glew.h>
#include "../../Assets/Texture3D/Texture3D.hpp"
#include "../../Core/Log/Log.hpp"

#include "../Renderer3D/Renderer3D.hpp"
#include "../RenderManager/RenderManager.hpp"


namespace Pine
{

	class CSkybox : public ISkybox
	{
	private:
		VertexArray* g_SkyboxVertexArray = nullptr;
		Texture3D* g_SkyboxCubemap = nullptr;
		Shader* g_SkyboxShader = nullptr;
	public:

		void Setup( ) override
		{
			Log->Debug( "Pine::Skybox->Setup( )" );

			const std::vector<float> skyboxVertices = 
			{
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f, -1.0f,
				 1.0f,  1.0f,  1.0f,
				 1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f, -1.0f,
				 1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				 1.0f, -1.0f,  1.0f
			};

			g_SkyboxVertexArray = new VertexArray( );

			g_SkyboxVertexArray->Create( );
			g_SkyboxVertexArray->Bind( );
			g_SkyboxVertexArray->StoreFloatBuffer( skyboxVertices, 3, 0 );

			g_SkyboxShader = Assets->GetAsset<Shader>( "Assets/Engine/Shaders/Skybox.shr" );
		}

		void Dispose( ) override
		{
			g_SkyboxVertexArray->Dispose( );
			delete g_SkyboxVertexArray;
		}

		void SetSkyboxCubemap( Texture3D* texture ) override
		{
			g_SkyboxCubemap = texture;
		}

		Texture3D* GetSkyboxCubemap( ) override
		{
			return g_SkyboxCubemap;
		}

		void Render( ) override
		{
			if ( !g_SkyboxCubemap ) 
			{
				return;
			}

			glDepthFunc( GL_LEQUAL );
			glActiveTexture( GL_TEXTURE0 );

			g_SkyboxShader->Use( );

			g_SkyboxVertexArray->Bind( );
			g_SkyboxCubemap->Bind( );

			Renderer3D->UpdateTextureBound( 0, g_SkyboxCubemap->GetId( ) );

			glDrawArrays( GL_TRIANGLES, 0, 36 );

			glDepthFunc( GL_LESS );

			RenderManager->GetRenderingContext( )->m_DrawCalls++;
		}
	};

	ISkybox* CreateSkyboxInterface( )
	{
		return new CSkybox( );
	}

}
