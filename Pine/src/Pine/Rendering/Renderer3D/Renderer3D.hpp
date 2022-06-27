#pragma once
#include "../../OpenGL/VertexArray/VertexArray.hpp"
#include "../../Assets/Mesh/Mesh.hpp"
#include "../../Assets/Terrain/Terrain.hpp"

namespace Pine
{
	class Camera;
	class Light;

	class IRenderer3D : public IInterface
	{
	public:
		// Set/Get the shader used for rendering
		virtual void SetShader( Shader* shader, std::uint32_t version = 0 ) = 0;
		virtual Shader* GetShader( ) = 0;

		// OpenGL wrappers
		virtual void SetStencilFunction( const int function, std::uint8_t mask ) = 0;
		virtual void SetStencilMask( std::uint8_t mask ) = 0;
		virtual void SetDepthTesting( bool value ) = 0;
		virtual void SetWireframeMode( bool value ) = 0;
		virtual void SetBackfaceCulling( bool value ) = 0;
		virtual void SetBlending( bool value ) = 0;

		// Used for very early debugging purposes.
		virtual void RenderVertexArray( const VertexArray* vao, int renderCount, bool indices ) = 0;

		// Prepare a mesh for rendering, won't render anything.
		virtual void PrepareMeshRendering( ) = 0;
		virtual void PrepareMesh( Mesh* mesh, Material* overrideMaterial = nullptr, std::uint8_t overrideStencilBuffer = 0x00 ) = 0;

		// Renders the prepared mesh via the transformation matrix.
		virtual void RenderMesh( const glm::mat4& transformationMatrix ) = 0;
		virtual void RenderMesh( Mesh* mesh, const glm::mat4& transformationMatrix ) = 0;
		virtual void RenderMeshInstanced( int count, glm::mat4* data ) = 0;

		// Terrain stuff
		virtual void PrepareTerrain( Terrain* terrain ) = 0;
		virtual void RenderTerrainChunk( const TerrainChunk& chunk ) = 0;

		// Updates the cached bound texture
		virtual	void UpdateTextureBound( int num, int value ) = 0;

		// Uploads matrices and from the camera, so the shaders that require them can use them.
		virtual void UploadCameraData( Camera* camera ) = 0;
		virtual void UploadCameraData( const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix ) = 0;

		// This below could and will be confusing, if we're rendering a scene we want to first call ResetLightData, as it will reset the light data we have on the CPU,
		// we then call PrepareLightData for all the lights in the scene, if we don't have any then don't call it at all. After all lights have been prepared then call
		// UploadLightData( ) as it will upload the prepared data to the GPU.

		virtual void ResetLightData( ) = 0;
		virtual void PrepareLightData( Light* light ) = 0;
		virtual void UploadLightData( ) = 0;
	};
}