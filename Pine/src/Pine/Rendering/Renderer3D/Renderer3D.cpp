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

#include <glm/gtx/fast_square_root.hpp>
#include <glm/trigonometric.hpp>

namespace Pine
{

    struct LightData_t
    {
        bool valid = false;
        glm::vec3 position;
    };

	class CRenderer3D : public IRenderer3D
	{
	private:
		// The mesh we're supposed to render.
		Mesh* g_CurrentRenderMesh = nullptr;

		// Shader stuff.
		Shader* g_CurrentShader = nullptr;
		Shader* g_TerrainShader = nullptr;
		UniformVariable* g_ShaderTransformationVariable = nullptr;
		UniformVariable* g_ShaderLightIndicesVariable = nullptr;

		std::uint32_t g_CurrentShaderVersion = 0;

		bool g_BackfaceCullingEnabled = true;
		bool g_WireframeModeEnabled = false;
		bool g_BlendingEnabled = false;

		// The texture we use if there is no texture applied, 1x1 white.
		Texture2D* g_DefaultTexture = nullptr;

        // If for some reason the mesh's material is nullptr
        Material* g_MissingMaterial = nullptr;

		// Some optimizations for OpenGL, we store stuff in CPU memory instead of querying the GPU each frame
		uint32_t g_CurrentBoundTexture[ 32 ] = { };
		uint8_t g_CurrentStencilMode = 0x00;

        // Lightning stuff
        bool m_LightSplittingRequired = false;
        LightData_t m_LightData[ 32 ];

        void ResetLights( )
        {
            std::memset(m_LightData, 0, sizeof (m_LightData));
        }

		// TODO: When and if this engine gets a little bit more advanced, we should get the nearest cube map
		int GetBestEnvironmentMap( )
		{
			if ( !Skybox->GetSkyboxCubemap( ) )
				return 0;

			return Skybox->GetSkyboxCubemap( )->GetId( );
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

            if ( !material )
            {
                material = g_MissingMaterial;
            }

			if ( overrideMaterial != nullptr )
			{
				material = overrideMaterial;
			}

			mesh->GetVertexArray( )->Bind( );

			// Figure out the correct shader version
			std::uint32_t shaderVersion = material->GetShaderProperties( );

			if ( material->GetRenderingMode( ) == MatRenderingMode::Discard )
				shaderVersion |= TransparencyDiscard;

			if ( material->GetShader( ) != g_CurrentShader || g_CurrentShaderVersion != shaderVersion )
				SetShader( material->GetShader( ), shaderVersion );

			SetBackfaceCulling( !( material->GetRenderFlags( ) & Pine::RenderFlags::DisableBackfaceCulling ) );
			SetWireframeMode( material->GetRenderFlags( ) & Pine::RenderFlags::RenderWireframe );
			SetBlending( material->GetRenderingMode( ) == MatRenderingMode::Transparent );

			if ( !g_CurrentShader )
				return;

			const Texture2D* diffuseTexture = g_DefaultTexture;

			if ( material->GetDiffuse( ) != nullptr )
			{
				diffuseTexture = material->GetDiffuse( );
			}

			// Only bind the texture if required.
			if ( g_CurrentBoundTexture[ 0 ] != diffuseTexture->GetId( ) )
			{
				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, diffuseTexture->GetId( ) );

				g_CurrentBoundTexture[ 0 ] = diffuseTexture->GetId( );
			}

			// Specular map texture
			const Texture2D* specularMapTexture = g_DefaultTexture;
			if ( material->GetSpecular( ) != nullptr )
			{
				specularMapTexture = material->GetSpecular( );
			}

			g_CurrentShader->GetUniformVariable( "materialSamplers.specular" )->LoadInteger( 1 );

			// Only bind the texture if required.
			if ( g_CurrentBoundTexture[ 1 ] != specularMapTexture->GetId( ) )
			{
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
			materialDataBuffer->shininiess = material->GetShininess( );
			materialDataBuffer->textureScale = material->GetTextureScale( );

			// Due to the amazing design of this game engine, the material uniform buffer SHOULD already be bound by now,
			// if not, I've fucked something up.
			UniformBuffers::GetMaterialUniformBuffer( )->UploadData( 0, sizeof( UniformBuffers::MaterialBufferData_t ), materialDataBuffer );

			// Apply correct stencil buffer mode
			if ( g_CurrentStencilMode != overrideStencilBuffer )
			{
                glStencilFunc( GL_ALWAYS, 1, overrideStencilBuffer );
                glStencilMask( overrideStencilBuffer );

                /*if (overrideStencilBuffer != 0)
                    glEnable(GL_STENCIL);
                else
                    glDisable(GL_STENCIL);*/

				g_CurrentStencilMode = overrideStencilBuffer;
			}

			g_CurrentRenderMesh = mesh;
		}

        void RenderMesh( Mesh* mesh, const glm::mat4& transformationMatrix ) override
        {
            g_CurrentRenderMesh = mesh;

            mesh->GetVertexArray( )->Bind( );

            RenderMesh( transformationMatrix );
        }

		void RenderMesh( const glm::mat4& transformationMatrix ) override
		{
			if ( g_CurrentRenderMesh == nullptr )
				return;

            if ( m_LightSplittingRequired )
            {
                if (g_ShaderLightIndicesVariable != nullptr)
                {
                    const auto meshPosition = glm::vec3(transformationMatrix[3]);

                    int lights[4] = { -1, -1, -1, -1 };

                    // Find the closest lights
                    for (int& light : lights)
                    {
                        float currentDistance = std::numeric_limits<float>::max();
                        int bestLight = -1;

                        for (int j = 1; j < g_CurrentDynamicLightCount;j++)
                        {
                            // I know this looks stupid but this code gotta be super-duper fast.
                            if (j == lights[0] || j == lights[1] || j == lights[2] || j == lights[3])
                                continue;

                            const float distance = glm::fastDistance(m_LightData[j].position, meshPosition);

                            if (currentDistance > distance)
                            {
                                currentDistance = distance;
                                bestLight = j;
                            }
                        }

                        if (bestLight != -1)
                            light = bestLight;

                        if (light == -1)
                        {
                            light = 31;
                        }
                    }

                    g_ShaderLightIndicesVariable->LoadVector4(glm::ivec4(lights[0], lights[1], lights[2], lights[3]));
                }
            }
            else
            {
                if (g_ShaderLightIndicesVariable != nullptr)
                {
                    g_ShaderLightIndicesVariable->LoadVector4(glm::ivec4(1, 2, 3, 4));
                }
            }

			if ( g_ShaderTransformationVariable != nullptr )
			{
				g_ShaderTransformationVariable->LoadMatrix4( transformationMatrix );
			}

			if ( g_CurrentRenderMesh->HasElementBuffer( ) )
			{
				glDrawElements( GL_TRIANGLES, g_CurrentRenderMesh->GetRenderCount( ), GL_UNSIGNED_INT, nullptr );
			}
			else
			{
				glDrawArrays( GL_TRIANGLES, 0, g_CurrentRenderMesh->GetRenderCount( ) );
			}

			RenderManager->GetRenderingContext( )->m_DrawCalls++;
		}

		void RenderMeshInstanced( int count, glm::mat4* data ) override
		{
			if ( g_CurrentRenderMesh == nullptr )
				return;

			memcpy( UniformBuffers::GetTransformBufferData( )->transform, data, sizeof( glm::mat4 ) * count );

			UniformBuffers::GetTransformDataUniformBuffer( )->UploadData( 0, sizeof( UniformBuffers::TransformData_t ), data );

			if ( g_CurrentRenderMesh->HasElementBuffer( ) )
			{
				glDrawElementsInstanced( GL_TRIANGLES, g_CurrentRenderMesh->GetRenderCount( ), GL_UNSIGNED_INT, nullptr, count );
			}
			else
			{
				glDrawArraysInstanced( GL_TRIANGLES, 0, g_CurrentRenderMesh->GetRenderCount( ), count );
			}

			RenderManager->GetRenderingContext( )->m_DrawCalls++;
		}

		void PrepareTerrain( Terrain* terrain ) override
		{
			g_TerrainShader->Use( );
		}

		void RenderTerrainChunk( const TerrainChunk& chunk ) override
		{
			RenderVertexArray( chunk.GetVertexArray( ), chunk.GetRenderCount( ), true );
		}

		void UpdateTextureBound( int num, int value ) override
		{
			g_CurrentBoundTexture[ num ] = value;
		}

		void UploadCameraData( Camera* camera ) override
		{
            UploadCameraData( camera->GetProjectionMatrix( ), camera->GetViewMatrix( ) );
		}

        void UploadCameraData( const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix ) override
        {
            UniformBuffers::GetMatrixBufferData( )->ProjectionMatrix = projectionMatrix;
            UniformBuffers::GetMatrixBufferData( )->ViewMatrix = viewMatrix;

            UniformBuffers::GetMatrixUniformBuffer( )->Bind( );
            UniformBuffers::GetMatrixUniformBuffer( )->UploadData( 0, sizeof( UniformBuffers::MatrixBufferData_t ), UniformBuffers::GetMatrixBufferData( ) );
        }

		void UploadLightData( ) override
		{
			UniformBuffers::GetLightsUniformBuffer( )->Bind( );
			UniformBuffers::GetLightsUniformBuffer( )->UploadData( 0, sizeof( UniformBuffers::LightBufferData_t ), UniformBuffers::GetLightsBufferData( ) );

            if ( g_CurrentDynamicLightCount >= 3 )
            {
                m_LightSplittingRequired = true;
            }
		}

		void PrepareMeshRendering( ) override
		{
			UniformBuffers::GetMaterialUniformBuffer( )->Bind( );
		}

		void PrepareLightData( Light* light ) override
		{
			// Make sure we're not overflowing the lights buffer
			if ( g_CurrentDynamicLightCount == 32 )
			{
				// This will spam the living fuck out of the console.
				Log->Warning( "Maximum level of dynamic lights reached." );

				return;
			}

			// If it's a directional light, always put it at index 0. There should only be one active directional light per scene anyway.
			// The other types of lights we put at (1 + g_CurrentDynamicLightCount), up till the max dynamic light count
			const int lightSlot = light->GetLightType( ) == LightType::Directional ? 0 : 1 + g_CurrentDynamicLightCount;

			UniformBuffers::GetLightsBufferData( )->lights[ lightSlot ].position = light->GetParent( )->GetTransform( )->GetPositionSum( );
			UniformBuffers::GetLightsBufferData( )->lights[ lightSlot ].rotation = glm::eulerAngles(light->GetParent( )->GetTransform( )->GetRotationSum( ));
			UniformBuffers::GetLightsBufferData( )->lights[ lightSlot ].color = light->GetLightColor( );
			UniformBuffers::GetLightsBufferData( )->lights[ lightSlot ].attenuation = light->GetAttenuation( );
			UniformBuffers::GetLightsBufferData( )->lights[ lightSlot ].cutOffAngle = light->GetLightType( ) == LightType::SpotLight ? light->GetSpotlightAngle( ) : -1.f;
			UniformBuffers::GetLightsBufferData( )->lights[ lightSlot ].cutOffSmoothness = UniformBuffers::GetLightsBufferData( )->lights[ lightSlot ].cutOffAngle + light->GetSpotlightSmoothness( );

            m_LightData[ lightSlot ].valid = true;
            m_LightData[ lightSlot ].position = light->GetParent( )->GetTransform( )->GetPositionSum( );

			if ( light->GetLightType( ) != LightType::Directional )
				g_CurrentDynamicLightCount++;
		}

		void ResetLightData( ) override
		{
			for ( auto& light : UniformBuffers::GetLightsBufferData( )->lights )
			{
				light.color = glm::vec3( 0.f, 0.f, 0.f );
				light.position = glm::vec3( 0.f, 0.f, 0.f );
				light.attenuation = glm::vec3( 0.f, 0.f, 0.f );
				light.cutOffAngle = 0.0f;
				light.cutOffSmoothness = 0.0f;
			}

			g_CurrentDynamicLightCount = 0;
            m_LightSplittingRequired = false;

            ResetLights( );
		}

		Shader* GetShader( ) override
		{
			return g_CurrentShader;
		}

		void SetBackfaceCulling( bool value ) override
		{
			if ( g_BackfaceCullingEnabled == value ) return;

			if ( value )
				glEnable( GL_CULL_FACE );
			else
				glDisable( GL_CULL_FACE );

			g_BackfaceCullingEnabled = value;
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
			if ( g_WireframeModeEnabled == value ) return;

			if ( value )
				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			else
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

			g_WireframeModeEnabled = value;
		}

		void SetBlending( bool value ) override
		{
			if ( g_BlendingEnabled == value ) return;

			if ( value )
				glEnable( GL_BLEND );
			else
				glDisable( GL_BLEND );

			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			g_BlendingEnabled = value;
		}

		void SetShader( Shader* shader, std::uint32_t version ) override
		{
			if ( !shader )
			{
				g_CurrentShader = nullptr;

				return;
			}

			g_CurrentShader = shader;
			g_CurrentShader->Use( version );
			g_CurrentShaderVersion = version;

			// Set cached uniform variables
			g_ShaderTransformationVariable = shader->GetUniformVariable( "transform" );
            g_ShaderLightIndicesVariable = shader->GetUniformVariable( "lightIndices" );
		}

		void Setup( ) override
		{
			Log->Debug( "Pine::Renderer3D->Setup( )" );

			//// Create default texture
			//float whiteData[ 4 ] = { 255, 255, 255, 255 };
			//
			//g_DefaultTexture = new Texture2D( );
			//g_DefaultTexture->CreateFromData( 1, 1, GL_RGBA, reinterpret_cast< void* >( &whiteData ) );

			//// Fake a texture being loaded if it's required elsewhere.
			//Assets->MapAsset( g_DefaultTexture, "Assets/Engine/DefaultTexture.png" );

			g_DefaultTexture = Assets->GetAsset<Texture2D>( "Assets/Engine/Textures/DefaultTexture.png" );
            g_MissingMaterial = Assets->GetAsset<Material>("Assets/Engine/Materials/MissingMaterial.mat");
			g_TerrainShader = Assets->GetAsset<Shader>( "Assets/Engine/Shaders/Terrain.shr" );
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
