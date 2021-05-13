#pragma once
#include "../../OpenGL/VertexArray/VertexArray.hpp"
#include "../../Assets/Mesh/Mesh.hpp"
#include "../../Assets/Terrain/Terrain.hpp"

namespace Pine::Renderer3D
{
	void Setup( );
	void Dispose( );

	// Set/Get the shader used for rendering
	void SetShader( Pine::Shader* shader );
	Pine::Shader* GetShader( );

	// Used for very early debugging purposes.
	void RenderVertexArray( const VertexArray* vao, int renderCount, bool indices );

	// Prepare a mesh for rendering, won't render anything.
	void PrepareMesh( Pine::Mesh* mesh );

	// Renders the prepared mesh via the transformation matrix.
	void RenderMesh( const glm::mat4& transformationMatrix );

	// Terrain stuff
	void PrepareTerrain( Pine::Terrain* terrain );
	void RenderTerrainChunk( const Pine::TerrainChunk& chunk );
	
	// Updates the cached bound texture
	void UpdateTextureBound( int num, int value );
	
}
