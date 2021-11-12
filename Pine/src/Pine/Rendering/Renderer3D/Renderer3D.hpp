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
		virtual void SetShader( Pine::Shader* shader ) = 0;
		virtual Pine::Shader* GetShader( ) = 0;

		// OpenGL wrappers
		virtual void SetStencilFunction( const int function, std::uint8_t mask ) = 0;
		virtual void SetStencilMask( std::uint8_t mask ) = 0;
		virtual void SetDepthTesting( bool value ) = 0;
		virtual void SetWireframeMode( bool value ) = 0;

		// Used for very early debugging purposes.
		virtual void RenderVertexArray( const VertexArray* vao, int renderCount, bool indices ) = 0;

		// Prepare a mesh for rendering, won't render anything.
		virtual void PrepareMesh( Pine::Mesh* mesh, Material* overrideMaterial = nullptr, std::uint8_t overrideStencilBuffer = 0x00 ) = 0;

		// Renders the prepared mesh via the transformation matrix.
		virtual void RenderMesh( const glm::mat4& transformationMatrix ) = 0;

		// Terrain stuff
		virtual void PrepareTerrain( Pine::Terrain* terrain ) = 0;
		virtual void RenderTerrainChunk( const Pine::TerrainChunk& chunk ) = 0;

		// Updates the cached bound texture
		virtual	void UpdateTextureBound( int num, int value ) = 0;

		// Low level shader interaction
		virtual void UploadCameraData( Pine::Camera* camera ) = 0;

		// This below could and will be confusing, if we're rendering a scene we want to first call ResetLightData, as it will reset the light data we have on the CPU,
		// we then call PrepareLightData for all the lights in the scene, if we don't have any then don't call it at all. After all lights have been prepared then call
		// UploadLightData( ) as it will upload the prepared data to the GPU.

		virtual void ResetLightData( ) = 0;
		virtual void PrepareLightData( Pine::Light* light ) = 0;
		virtual void UploadLightData( ) = 0;

		virtual void PrepareMeshRendering( ) = 0;
	};


}