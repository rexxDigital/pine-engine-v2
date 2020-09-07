#include "PreviewManager.hpp"


#include <glm/ext/matrix_transform.hpp>
#include <GL/glew.h>
#include <Pine/Entity/Entity.hpp>
#include <Pine/Components/Camera/Camera.hpp>
#include <Pine/OpenGL/FrameBuffer/FrameBuffer.hpp>
#include <Pine/UniformBuffers/UniformBuffers.hpp>

#include "Pine/Rendering/Renderer3D/Renderer3D.hpp"
#include "Pine/Rendering/Skybox/Skybox.hpp"

namespace {
	Pine::FrameBuffer* g_FrameBuffer = nullptr;

	Pine::Entity* g_FakeCameraEntity = nullptr;

	Pine::Camera* g_FakeCamera = nullptr;

	glm::mat4 g_EntityTransformationMatrix = glm::mat4( 1.f );

	float g_RotationY = 0.f;
}

void PreviewManager::Setup( ) {

	// Create fake camera
	g_FakeCameraEntity = new Pine::Entity( 0 );
	g_FakeCameraEntity->AddComponent( new Pine::Camera( ) );
	//g_FakeCameraEntity->GetTransform( )->Position.z = -2.f;
	//g_FakeCameraEntity->GetTransform( )->Position.y = -0.2f;

	g_FakeCamera = reinterpret_cast< Pine::Camera* >( g_FakeCameraEntity->GetComponents( )[ 1 ] );

	g_EntityTransformationMatrix = glm::mat4( 1.f );
	g_EntityTransformationMatrix = glm::translate( g_EntityTransformationMatrix, glm::vec3( 0.f, 0.f, 0.f ) );

	//g_EntityTransformationMatrix = glm::rotate( g_EntityTransformationMatrix, glm::radians( 45.f ), glm::vec3( 1.f, 0.f, 0.f ) );
	//g_EntityTransformationMatrix = glm::rotate( g_EntityTransformationMatrix, glm::radians( 45.f ), glm::vec3( 0.f, 1.f, 0.f ) );
	//g_EntityTransformationMatrix = glm::rotate( g_EntityTransformationMatrix, glm::radians( 0.f ), glm::vec3( 0.f, 0.f, 1.f ) );

}

void PreviewManager::PrepareRender( Pine::FrameBuffer* frameBuffer ) {
	g_FrameBuffer = frameBuffer;
	g_FrameBuffer->Bind( );

	glClearColor( 0.f, 0.f, 0.f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glViewport( 0, 0, frameBuffer->GetWidth( ), frameBuffer->GetHeight( ) );

	glEnable( GL_DEPTH_TEST );

	g_FakeCamera->OnRender( );

	// Prepare data
	Pine::UniformBuffers::GetMatrixBufferData( )->ProjectionMatrix = g_FakeCamera->GetProjectionMatrix( );
	Pine::UniformBuffers::GetMatrixBufferData( )->ViewMatrix = g_FakeCamera->GetViewMatrix( );

	for ( int i = 0; i < 5; i++ ) {
		Pine::UniformBuffers::GetLightsBufferData( )->lights[ i ].color = glm::vec3( 0.f, 0.f, 0.f );
		Pine::UniformBuffers::GetLightsBufferData( )->lights[ i ].position = glm::vec3( 0.f, 0.f, 0.f );
	}

	Pine::UniformBuffers::GetLightsBufferData( )->lights[ 0 ].color = glm::vec3( 1.f, 1.f, 1.f );
	Pine::UniformBuffers::GetLightsBufferData( )->lights[ 0 ].position = glm::vec3( 0.f, 0.f, -100.f );

	// Upload to the uniform buffer
	Pine::UniformBuffers::GetMatrixUniformBuffer( )->Bind( );
	Pine::UniformBuffers::GetMatrixUniformBuffer( )->UploadData( 0, sizeof( Pine::UniformBuffers::MatrixBufferData_t ), Pine::UniformBuffers::GetMatrixBufferData( ) );

	Pine::UniformBuffers::GetLightsUniformBuffer( )->Bind( );
	Pine::UniformBuffers::GetLightsUniformBuffer( )->UploadData( 0, sizeof( Pine::UniformBuffers::LightBufferData_t ), Pine::UniformBuffers::GetLightsBufferData( ) );

	Pine::UniformBuffers::GetMaterialUniformBuffer( )->Bind( );
}

void PreviewManager::FinishRender( ) {
	Pine::Skybox::Render( );

	// Reset the frame buffer for rendering.
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void PreviewManager::RenderMesh( Pine::Mesh* mesh )
{
	Pine::Renderer3D::PrepareMesh( mesh );
	Pine::Renderer3D::RenderMesh( g_EntityTransformationMatrix );
}

void PreviewManager::Dispose( ) {
	delete g_FakeCameraEntity;
}

void PreviewManager::UseStaticTransformationMatrix( Pine::Mesh* mesh )
{
	g_EntityTransformationMatrix = glm::mat4( 1.f );
	g_EntityTransformationMatrix = glm::translate( g_EntityTransformationMatrix, glm::vec3( 0.f, 0.f, ( mesh->GetMins( ) - mesh->GetMaxs( ) ).z * 1.5f ) );
}

void PreviewManager::UseRotatingTransformationMatrix( Pine::Mesh* mesh )
{
	g_EntityTransformationMatrix = glm::mat4( 1.f );
	g_EntityTransformationMatrix = glm::translate( g_EntityTransformationMatrix, glm::vec3( 0.f, 0.f, ( mesh->GetMins( ) - mesh->GetMaxs( ) ).z * 1.5f ) );
	g_EntityTransformationMatrix = glm::rotate( g_EntityTransformationMatrix, glm::radians( g_RotationY ), glm::vec3( 0.f, 1.f, 0.f ) );

	g_RotationY += 0.3f;

	if ( g_RotationY > 180.f )
	{
		g_RotationY = -180.f;
	}
}

glm::mat4& PreviewManager::GetRenderTransformationMatrix( ) {
	return g_EntityTransformationMatrix;
}
