#include "Renderer3D.hpp"
#include <GL/glew.h>

#include "../Skybox/Skybox.hpp"
#include "../UniformBuffers/UniformBuffers.hpp"
#include "../../Assets/Texture3D/Texture3D.hpp"
#include "../../Core/Log/Log.hpp"
#include "../RenderManager/RenderManager.hpp"

namespace {
	// The mesh we're supposed to render.
	Pine::Mesh* g_CurrentRenderMesh = nullptr;

	// Shader stuff.
	Pine::Shader* g_CurrentShader = nullptr;
	Pine::Shader* g_TerrainShader = nullptr;
	Pine::UniformVariable* g_ShaderTransformationVariable = nullptr;

	// The texture we use if there is no texture applied, 1x1 white.
	Pine::Texture2D* g_DefaultTexture = nullptr;

	// Some optimizations for OpenGL's current texture.
	int g_CurrentBoundTexture[ 32 ] = { };

	// TODO: When this engine gets a little bit more advanced, we should get the nearest cube map
	// or something for this function. Time will tell.
	int GetBestEnvironmentMap( )
	{
		return Pine::Skybox::GetSkyboxCubemap( )->GetId(  );
	}
}

void Pine::Renderer3D::RenderVertexArray( const VertexArray* vao, int renderCount, bool indices )
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

void Pine::Renderer3D::PrepareMesh( Pine::Mesh* mesh ) {
	auto material = mesh->GetMaterial( );

	mesh->GetVertexArray( )->Bind( );

	if ( material->GetShader( ) != g_CurrentShader ) {
		SetShader( material->GetShader( ) );
	}
	
	if ( !g_CurrentShader )
		return;
	
	// Diffuse texture
	Texture2D* diffuseTexture = g_DefaultTexture;
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
	Texture2D* specularMapTexture = g_DefaultTexture;
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
	if (g_CurrentBoundTexture[ 2 ] != bestEnvironmentMap )
	{
		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_3D, bestEnvironmentMap );

		g_CurrentBoundTexture[ 2 ] = bestEnvironmentMap;
	}

	// Apply material data
	auto materialDataBuffer = UniformBuffers::GetMaterialBufferData( );

	materialDataBuffer->diffuseColor = material->DiffuseColor( );
	materialDataBuffer->specularColor = material->SpecularColor( );
	materialDataBuffer->ambientColor = material->AmbientColor( );
	materialDataBuffer->shininiess = material->GetShininiess( );
	materialDataBuffer->textureScale = material->GetTextureScale( );

	// Due to the amazing design of this game engine, the material uniform buffer SHOULD already be bound by now,
	// if not, I've fucked something up.
	UniformBuffers::GetMaterialUniformBuffer( )->UploadData( 0, sizeof( UniformBuffers::MaterialBufferData_t ), materialDataBuffer );

	g_CurrentRenderMesh = mesh;
	g_CurrentShader->Use( );
}

void Pine::Renderer3D::RenderMesh( const glm::mat4& transformationMatrix ) {
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
	
	RenderManager::GetRenderingContext( )->m_DrawCalls++;
}

void Pine::Renderer3D::PrepareTerrain( Pine::Terrain* terrain )
{
	g_TerrainShader->Use( );
}

void Pine::Renderer3D::RenderTerrainChunk( const Pine::TerrainChunk& chunk )
{
	RenderVertexArray( chunk.GetVertexArray( ), chunk.GetRenderCount( ), true );
}

void Pine::Renderer3D::UpdateTextureBound( int num, bool value )
{
	g_CurrentBoundTexture[ num ] = value;
}

Pine::Shader* Pine::Renderer3D::GetShader( ) {
	return g_CurrentShader;
}

void Pine::Renderer3D::SetShader( Pine::Shader* shader ) {
	if ( !shader )
		return;
	
	g_CurrentShader = shader;

	// Set cached uniform variables
	g_ShaderTransformationVariable = shader->GetUniformVariable( "transform" );
}

void Pine::Renderer3D::Setup( ) {
	Log::Debug( "Pine::Renderer3D::Setup()" );
	
	// Create default texture
	char whiteData[ 4 ] = { 255, 255, 255, 255 };
	
	g_DefaultTexture = new Texture2D( );
	g_DefaultTexture->CreateFromData( 1, 1, GL_RGBA, reinterpret_cast< void* >( &whiteData ) );

	// Fake a texture being loaded if it's required elsewhere.
	Assets::MapAsset( g_DefaultTexture, "Assets\\Engine\\DefaultTexture" );
	
	g_TerrainShader = Assets::GetAsset<Shader>( "Assets\\Engine\\Shaders\\Terrain.shr" );
}

void Pine::Renderer3D::Dispose( ) {
}