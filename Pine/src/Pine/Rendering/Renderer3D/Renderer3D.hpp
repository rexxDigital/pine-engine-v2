#pragma once
#include "../../OpenGL/VertexArray/VertexArray.hpp"
#include "../../Assets/Mesh/Mesh.hpp"
#include "../../Assets/Terrain/Terrain.hpp"

namespace Pine
{
	class Camera;
	class Light;
}

namespace Pine::Renderer3D
{
	void Setup( );
	void Dispose( );

	// Set/Get the shader used for rendering
	void SetShader( Pine::Shader* shader );
	Pine::Shader* GetShader( );

	// OpenGL wrappers
	void SetStencilFunction( const int function, std::uint8_t mask );
	void SetStencilMask( std::uint8_t mask );
	void SetDepthTesting( bool value );

	// Used for very early debugging purposes.
	void RenderVertexArray( const VertexArray* vao, int renderCount, bool indices );

	// Prepare a mesh for rendering, won't render anything.
	void PrepareMesh( Pine::Mesh* mesh, Material* overrideMaterial = nullptr, std::uint8_t overrideStencilBuffer = 0x00 );

	// Renders the prepared mesh via the transformation matrix.
	void RenderMesh( const glm::mat4& transformationMatrix );

	// Terrain stuff
	void PrepareTerrain( Pine::Terrain* terrain );
	void RenderTerrainChunk( const Pine::TerrainChunk& chunk );
	
	// Updates the cached bound texture
	void UpdateTextureBound( int num, int value );

	// Low level shader interaction
	void UploadCameraData( Pine::Camera* camera );

	// This below could and will be confusing, if we're rendering a scene we want to first call ResetLightData, as it will reset the light data we have on the CPU,
	// we then call PrepareLightData for all the lights in the scene, if we don't have any then don't call it at all. After all lights have been prepared then call
	// UploadLightData( ) as it will upload the prepared data to the GPU.
	
	void ResetLightData( );
	void PrepareLightData( Pine::Light* light );
	void UploadLightData( );

	void PrepareMeshRendering( );
	
}
