#include "PostProcessing.hpp"

#include <random>

#include "../../OpenGL/FrameBuffer/FrameBuffer.hpp"

#include <GL/glew.h>
#include <glm/trigonometric.hpp>

#include "../../Assets/Shader/Shader.hpp"
#include "../../OpenGL/VertexArray/VertexArray.hpp"
#include "../Renderer3D/Renderer3D.hpp"
#include "../RenderManager/RenderManager.hpp"

namespace
{
	float lerp( float a, float b, float f )
	{
		return a + f * ( b - a );
	}
}

namespace
{
	Pine::FrameBuffer* g_PostProcessingBuffer = nullptr;
	Pine::VertexArray* g_ScreenQuad = nullptr;
	Pine::Shader* g_PostProcessingShader = nullptr;
	Pine::Texture2D* g_NoiseTexture = nullptr;

	std::vector<glm::vec3> g_AOSampleKernels;

	void SetupAmbientOcclusion( )
	{
		// Generate sample kernels
		std::uniform_real_distribution<float> randomFloat( 0.0, 1.0 ); // random floats between [0.0, 1.0]
		std::default_random_engine generator;

		for ( unsigned int i = 0; i < 64; ++i )
		{
			glm::vec3 sample(
				randomFloat( generator ) * 2.0 - 1.0,
				randomFloat( generator ) * 2.0 - 1.0,
				randomFloat( generator )
			);

			sample = glm::normalize( sample );
			sample *= randomFloat( generator );

			float scale = ( float ) i / 64.0;
			scale = lerp( 0.1f, 1.0f, scale * scale );

			sample *= scale;

			g_AOSampleKernels.push_back( sample );
		}

		std::vector<glm::vec3> ssaoNoise;
		for ( unsigned int i = 0; i < 16; i++ )
		{
			glm::vec3 noise(
				randomFloat( generator ) * 2.0 - 1.0,
				randomFloat( generator ) * 2.0 - 1.0,
				0.0f );
			ssaoNoise.push_back( noise );
		}

		g_NoiseTexture = new Pine::Texture2D;
		g_NoiseTexture->CreateFromData( 4, 4, GL_RGB, &ssaoNoise[ 0 ] );

		g_PostProcessingShader->GetUniformVariable( "aspectRatio" )->LoadFloat( static_cast< float >( Pine::RenderManager::GetRenderingContext( )->m_Width ) / static_cast< float >( Pine::RenderManager::GetRenderingContext( )->m_Height ) );

		for ( int i = 0; i < 64; i++ )
		{
			g_PostProcessingShader->GetUniformVariable( "samples[" + std::to_string( i ) + "]" )->LoadVector3( g_AOSampleKernels[ i ] );
		}
	}

}

void Pine::PostProcessing::Setup( )
{
	// Setup rendering frame buffer
	g_PostProcessingBuffer = new Pine::FrameBuffer;
	g_PostProcessingBuffer->Create( 1600, 900, true );

	// Setup the quad vertex array

	g_ScreenQuad = new Pine::VertexArray;
	g_ScreenQuad->Create( );
	g_ScreenQuad->Bind( );

	const std::vector<float> quads = {
		-1.f, 1.f, 0.f,//v0
		-1.f, -1.f, 0.f,//v1
		1.f, -1.f, 0.f,//v2
		1.f, 1.f, 0.f,//v3
	};

	const std::vector<int> indices = {
		0,1,3,
		3,1,2
	};

	const std::vector<float> uvs = {
		0,0,
		0,1,
		1,1,
		1,0
	};

	g_ScreenQuad->StoreFloatBuffer( quads, 3, 0 );
	g_ScreenQuad->StoreFloatBuffer( uvs, 2, 2 );
	g_ScreenQuad->StoreIntBuffer( indices );

	// Prepare post processing shader
	g_PostProcessingShader = Pine::Assets::GetAsset<Pine::Shader>( "Assets\\Engine\\Shaders\\PostProcessing.shr" );

	g_PostProcessingShader->Use( );
	g_PostProcessingShader->GetUniformVariable( "fragColor" )->LoadInteger( 0 );
	g_PostProcessingShader->GetUniformVariable( "fragNormal" )->LoadInteger( 2 );
	g_PostProcessingShader->GetUniformVariable( "fragDepth" )->LoadInteger( 3 );
	g_PostProcessingShader->GetUniformVariable( "noiseTexture" )->LoadInteger( 4 );

	// Prepare ambient occlusion
	SetupAmbientOcclusion( );
}

void Pine::PostProcessing::Dispose( )
{
	g_PostProcessingBuffer->Dispose( );
	g_ScreenQuad->Dispose( );

	delete g_ScreenQuad;
	delete g_PostProcessingBuffer;
}

void Pine::PostProcessing::Render( )
{
	// At this point, the target frame buffer is already bound, so we don't need to worry about that.

	// Clear the buffers
	glClearColor( 0.f, 0.f, 0.f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Reset the viewport size.
	glViewport( 0, 0, 1600, 900 );

	// Disable depth test
	glDisable( GL_DEPTH_TEST );

	// Bind the vertex array and the shader program
	g_ScreenQuad->Bind( );
	g_PostProcessingShader->Use( );

	const auto cam = RenderManager::GetRenderingContext( )->m_Camera;

	if ( !cam )
		return;

	g_PostProcessingShader->GetUniformVariable( "tanHalfFOV" )->LoadFloat( tanf( glm::radians( cam->GetFieldOfView( ) / 2.0f ) ) );

	// Bind frame buffer
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, g_PostProcessingBuffer->GetTextureId( ) );

	Renderer3D::UpdateTextureBound( 0, g_PostProcessingBuffer->GetTextureId( ) );

	// Bind normal buffer
	glActiveTexture( GL_TEXTURE2 );
	glBindTexture( GL_TEXTURE_2D, g_PostProcessingBuffer->GetNormalBufferId( ) );

	Renderer3D::UpdateTextureBound( 2, g_PostProcessingBuffer->GetNormalBufferId( ) );

	// Bind frame buffer
	glActiveTexture( GL_TEXTURE3 );
	glBindTexture( GL_TEXTURE_2D, g_PostProcessingBuffer->GetDepthId( ) );

	Renderer3D::UpdateTextureBound( 3, g_PostProcessingBuffer->GetDepthId( ) );

	glActiveTexture( GL_TEXTURE4 );
	glBindTexture( GL_TEXTURE_2D, g_NoiseTexture->GetId( ) );

	// Just do a simple render call
	glDrawElements( GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr );
}

Pine::FrameBuffer* Pine::PostProcessing::GetRenderBuffer( )
{
	return g_PostProcessingBuffer;
}
