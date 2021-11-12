#include "Renderer3D.hpp"
#include <GL/glew.h>

#include "../Skybox/Skybox.hpp"
#include "../UniformBuffers/UniformBuffers.hpp"
#include "../../Assets/Texture3D/Texture3D.hpp"
#include "../../Core/Log/Log.hpp"
#include "../RenderManager/RenderManager.hpp"

#include "../../Entity/Entity.hpp"
#include "../../Components/Transform/Transform.hpp"
#include "../../Components/Light/Light.hpp"
#include "../../Components/Camera/Camera.hpp"

namespace Pine
{

	class CRenderer3D : public IRenderer3D
	{
	private:
		// The mesh we're supposed to render.
		Pine::Mesh* g_CurrentRenderMesh = nullptr;

		// Shader stuff.
		Pine::Shader* g_CurrentShader = nullptr;
		Pine::Shader* g_TerrainShader = nullptr;
		Pine::UniformVariable* g_ShaderTransformationVariable = nullptr;

		// The texture we use if there is no texture applied, 1x1 white.
		Pine::Texture2D* g_DefaultTexture = nullptr;

		// Some optimizations for OpenGL, we store stuff in CPU memory instead of querying the GPU each frame
		uint32_t g_CurrentBoundTexture[ 32 ] = { };
		uint8_t g_CurrentStencilMode = 0x00;


		// TODO: When this engine gets a little bit more advanced, we should get the nearest cube map
		// or something for this function. Time will tell.
		int GetBestEnvironmentMap( )
		{
			return Pine::Skybox->GetSkyboxCubemap( )->GetId( );
		}

		int g_CurrentDynamicLightCount = 0;

	public:

		void RenderVertexArray( const VertexArray* vao, int renderCount, bool indices ) override
		{
			vao->Bind( );

			if ( indices )
			{
				glDrawElements( GL_TRIANGLES, renderCount, GL_UNSIGNED_INT, nullptr );
			}
			else
			{
				glDrawArrays( GL_TRIANGLES, 0, renderCount );
			}
		}

		void PrepareMesh( Mesh* mesh, Material* overrideMaterial, std::uint8_t overrideStencilBuffer ) override
		{
			auto material = mesh->GetMaterial( );

			if ( overrideMaterial != nullptr )
			{
				material = overrideMaterial;
			}

			if ( overrideStencilBuffer != 0x00 )
			{
				if ( g_CurrentStencilMode == 0x00 )
				{
					glStencilFunc( GL_ALWAYS, 1, overrideStencilBuffer );
					glStencilMask( overrideStencilBuffer );

					g_CurrentStencilMode = overrideStencilBuffer;
				}
			}
			else
			{
				if ( g_CurrentStencilMode != 0x00 )
				{
					glStencilFunc( GL_ALWAYS, 0, 0xFF );
					glStencilMask( 0x00 );
					g_CurrentStencilMode = 0x00;
				}
			}

			mesh->GetVertexArray( )->Bind( );

			if ( material->GetShader( ) != g_CurrentShader ) {
				SetShader( material->GetShader( ) );
			}

			if ( !g_CurrentShader )
				return;

			// Diffuse texture
			const Texture2D* diffuseTexture = g_DefaultTexture;
			if ( material->GetDiffuse( ) != nullptr ) {
				diffuseTexture = material->GetDiffuse( );
			}

			// Only bind the texture if required.
			if ( g_CurrentBoundTexture[ 0 ] != diffuseTexture->GetId( ) ) {
				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, diffuseTexture->GetId( ) );

				g_CurrentBoundTexture[ 0 ] = diffuseTexture->GetId( );
			}

			// Specular map texture
			const Texture2D* specularMapTexture = g_DefaultTexture;
			if ( material->GetSpecular( ) != nullptr ) {
				specularMapTexture = material->GetSpecular( );
			}

			g_CurrentShader->GetUniformVariable( "materialSamplers.specular" )->LoadInteger( 1 );

			// Only bind the texture if required.
			if ( g_CurrentBoundTexture[ 1 ] != specularMapTexture->GetId( ) ) {
				glActiveTexture( GL_TEXTURE1 );
				glBindTexture( GL_TEXTURE_2D, specularMapTexture->GetId( ) );

				g_CurrentBoundTexture[ 1 ] = specularMapTexture->GetId( );
			}

			const int bestEnvironmentMap = GetBestEnvironmentMap( );

			// Bind environment map
			if ( g_CurrentBoundTexture[ 2 ] != bestEnvironmentMap )
			{
				glActiveTexture( GL_TEXTURE2 );
				glBindTexture( GL_TEXTURE_3D, bestEnvironmentMap );

				g_CurrentBoundTexture[ 2 ] = bestEnvironmentMap;
			}

			// Apply material data
			const auto materialDataBuffer = UniformBuffers::GetMaterialBufferData( );

			materialDataBuffer->diffuseColor = material->DiffuseColor( );
			materialDataBuffer->specularColor = material->SpecularColor( );
			materialDataBuffer->ambientColor = material->AmbientColor( );
			materialDataBuffer->shininiess = material->GetShininiess( );
			materialDataBuffer->textureScale = material->GetTextureScale( );

			// Due to the amazing design of this game engine, the material uniform buffer SHOULD already be bound by now,
			// if not, I've fucked something up.
			UniformBuffers::GetMaterialUniformBuffer( )->UploadData( 0, sizeof( UniformBuffers::MaterialBufferData_t ), materialDataBuffer );

			g_CurrentShader->Use( );
			g_CurrentRenderMesh = mesh;
		}

		void RenderMesh( const glm::mat4& transformationMatrix ) override
		{
			if ( g_CurrentRenderMesh == nullptr )
				return;

			if ( g_ShaderTransformationVariable != nullptr ) {
				g_ShaderTransformationVariable->LoadMatrix4( transformationMatrix );
			}

			if ( g_CurrentRenderMesh->HasElementBuffer( ) ) {
				glDrawElements( GL_TRIANGLES, g_CurrentRenderMesh->GetRenderCount( ), GL_UNSIGNED_INT, nullptr );
			}
			else {
				glDrawArrays( GL_TRIANGLES, 0, g_CurrentRenderMesh->GetRenderCount( ) );
			}

			RenderManager->GetRenderingContext( )->m_DrawCalls++;
		}

		void PrepareTerrain( Pine::Terrain* terrain ) override
		{
			g_TerrainShader->Use( );
		}

		void RenderTerrainChunk( const Pine::TerrainChunk& chunk ) override
		{
			RenderVertexArray( chunk.GetVertexArray( ), chunk.GetRenderCount( ), true );
		}

		void UpdateTextureBound( int num, int value ) override
		{
			g_CurrentBoundTexture[ num ] = value;
		}

		void UploadCameraData( Pine::Camera* camera ) override
		{
			UniformBuffers::GetMatrixBufferData( )->ProjectionMatrix = camera->GetProjectionMatrix( );
			UniformBuffers::GetMatrixBufferData( )->ViewMatrix = camera->GetViewMatrix( );

			UniformBuffers::GetMatrixUniformBuffer( )->Bind( );
			UniformBuffers::GetMatrixUniformBuffer( )->UploadData( 0, sizeof( UniformBuffers::MatrixBufferData_t ), UniformBuffers::GetMatrixBufferData( ) );
		}

		void UploadLightData( ) override
		{
			UniformBuffers::GetLightsUniformBuffer( )->Bind( );
			UniformBuffers::GetLightsUniformBuffer( )->UploadData( 0, sizeof( UniformBuffers::LightBufferData_t ), UniformBuffers::GetLightsBufferData( ) );
		}

		void PrepareMeshRendering( ) override
		{
			UniformBuffers::GetMaterialUniformBuffer( )->Bind( );
		}

		void PrepareLightData( Pine::Light* light ) override
		{
			// If it's a directional light, always put it at index 0. There should only be one active directional light per scene anyway.
			// The other types of lights we put at (1 + g_CurrentDynamicLightCount), up till the max dynamic light count

			if ( g_CurrentDynamicLightCount == 3 )
			{
				// This will spam the living fuck out of the console.
				Log->Warning( "Maximum level of dynamic lights reached." );

				return;
			}

			if ( light->GetLightType( ) == Pine::ELightType::Directional )
			{
				UniformBuffers::GetLightsBufferData( )->lights[ 0 ].position = light->GetParent( )->GetTransform( )->Position;
				UniformBuffers::GetLightsBufferData( )->lights[ 0 ].rotation = glm::normalize( light->GetParent( )->GetTransform( )->Rotation );
				UniformBuffers::GetLightsBufferData( )->lights[ 0 ].color = light->GetLightColor( );
				UniformBuffers::GetLightsBufferData( )->lights[ 0 ].attenuation = light->GetAttenuation( );
			}
			else
			{

				UniformBuffers::GetLightsBufferData( )->lights[ 1 + g_CurrentDynamicLightCount ].position = light->GetParent( )->GetTransform( )->Position;
				UniformBuffers::GetLightsBufferData( )->lights[ 1 + g_CurrentDynamicLightCount ].rotation = glm::normalize( light->GetParent( )->GetTransform( )->Rotation );
				UniformBuffers::GetLightsBufferData( )->lights[ 1 + g_CurrentDynamicLightCount ].color = light->GetLightColor( );
				UniformBuffers::GetLightsBufferData( )->lights[ 1 + g_CurrentDynamicLightCount ].attenuation = light->GetAttenuation( );

			}

			g_CurrentDynamicLightCount++;

		}

		void ResetLightData( ) override
		{
			for ( int i = 0; i < 4; i++ ) {
				UniformBuffers::GetLightsBufferData( )->lights[ i ].color = glm::vec3( 0.f, 0.f, 0.f );
				UniformBuffers::GetLightsBufferData( )->lights[ i ].position = glm::vec3( 0.f, 0.f, 0.f );
				UniformBuffers::GetLightsBufferData( )->lights[ i ].attenuation = glm::vec3( 0.f, 0.f, 0.f );
			}

			g_CurrentDynamicLightCount = 0;
		}

		Pine::Shader* GetShader( ) override
		{
			return g_CurrentShader;
		}

		void SetStencilFunction( const int function, const std::uint8_t mask ) override
		{
			glStencilFunc( function, 1, mask );
		}

		void SetStencilMask( const std::uint8_t mask ) override
		{
			glStencilMask( mask );
		}

		void SetDepthTesting( const bool value ) override
		{
			if ( value )
				glEnable( GL_DEPTH_TEST );
			else
				glDisable( GL_DEPTH_TEST );
		}

		void SetWireframeMode( const bool value ) override
		{
			if ( value )
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			else
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}

		void SetShader( Pine::Shader* shader ) override
		{
			if ( !shader )
				return;

			g_CurrentShader = shader;

			// Set cached uniform variables
			g_ShaderTransformationVariable = shader->GetUniformVariable( "transform" );
		}

		void Setup( ) override
		{
			Log->Debug( "Pine::Renderer3D->Setup( )" );

			//// Create default texture
			//char whiteData[ 4 ] = { 255, 255, 255, 255 };
			//
			//g_DefaultTexture = new Texture2D( );
			//g_DefaultTexture->CreateFromData( 1, 1, GL_RGBA, reinterpret_cast< void* >( &whiteData ) );

			//// Fake a texture being loaded if it's required elsewhere.
			//Assets->MapAsset( g_DefaultTexture, "Assets\\Engine\\DefaultTexture.png" );

			//g_DefaultTexture = Assets->GetAsset<Texture2D>( "Assets\\Engine\\DefaultTexture.png" );

			g_DefaultTexture = Assets->GetAsset<Texture2D>( "Assets\\Engine\\DefaultTexture.png" );

			g_TerrainShader = Assets->GetAsset<Shader>( "Assets\\Engine\\Shaders\\Terrain.shr" );
		}

		void Dispose( ) override
		{

		}
	};

	IRenderer3D* CreateRenderer3DInterface( )
	{
		return new CRenderer3D( );
	}

}
