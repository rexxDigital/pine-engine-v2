#include "PostProcessing.hpp"
#include "../../OpenGL/FrameBuffer/FrameBuffer.hpp"

#include <GL/glew.h>


#include "../../Assets/Shader/Shader.hpp"
#include "../../OpenGL/VertexArray/VertexArray.hpp"

namespace
{
	Pine::FrameBuffer* g_PostProcessingBuffer = nullptr;
	Pine::VertexArray* g_ScreenQuad = nullptr;
	Pine::Shader* g_PostProcessingShader = nullptr;
}

void Pine::PostProcessing::Setup( )
{
	// Setup rendering frame buffer
	g_PostProcessingBuffer = new Pine::FrameBuffer;
	g_PostProcessingBuffer->Create( 1600, 900 );

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

	// Bind frame buffer
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, g_PostProcessingBuffer->GetTextureId( ) );
	
	// Just do a simple render call
	glDrawElements( GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr );
}

Pine::FrameBuffer* Pine::PostProcessing::GetRenderBuffer( )
{
	return g_PostProcessingBuffer;
}
