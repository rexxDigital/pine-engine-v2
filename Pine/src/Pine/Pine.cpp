#include "Pine.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Rendering/RenderManager/RenderManager.hpp"
#include "Rendering/UniformBuffers/UniformBuffers.hpp"
#include "Entitylist/EntityList.hpp"
#include "Gui/Gui.hpp"
#include "Rendering/Skybox/Skybox.hpp"
#include "Assets/Texture3D/Texture3D.hpp"

bool Pine::Setup( )
{
	Log::Message( "Setting up Pine..." );

	if ( !glfwInit( ) )
	{
		Log::Fatal( "Failed to initialize GLFW." );
		return false;
	}

	if ( !Window::Internal::Create( ) )
	{
		// We already print out an error message in the function, no need to do it again.
		return false;
	}

	glewExperimental = true;

	if ( glewInit( ) != GLEW_OK )
	{
		Log::Fatal( "Failed to initialize GLEW." );
		return false;
	}

	Log::Message( "Loading engine shaders..." );

	// We will have to setup the uniform buffers first.
	UniformBuffers::Setup( );

	if ( Assets::LoadFromDirectory( "Assets\\Engine\\Shaders" ) == 0 ) {
		Log::Fatal( "Failed to load engine shaders." );
		return false;
	}

	Log::Message( "Loading engine assets..." );

	if ( Assets::LoadFromDirectory( "Assets\\Engine" ) == 0 ) {
		Log::Fatal( "Failed to load engine assets." );
		return false;
	}

	Renderer3D::Setup( );
	RenderManager::SetRenderingContext( CreateDefaultRenderingContext( ) );

	Skybox::Setup( );
	Skybox::SetSkyboxCubemap( Assets::GetAsset<Pine::Texture3D>( "Assets\\Engine\\Skyboxes\\DefaultSkybox.cmap" ) );

	Gui::Setup( );

	Log::Message( "Pine was successfully initialized!" );

	return true;
}

void Pine::Run( )
{
	Window::Show( );
	Window::UpdateCachedSize( );

	EntityList::RunOnSetup( );

	const auto window = Window::Internal::GetWindowPointer( );

	glEnable( GL_DEPTH_TEST );

	while ( !glfwWindowShouldClose( window ) )
	{
		RenderManager::Run( );

		Gui::Render( );

		glfwSwapBuffers( window );
		glfwPollEvents( );
	}
}

void Pine::Terminate( )
{
	Assets::Dispose( );
	UniformBuffers::Dispose( );
	Renderer3D::Dispose( );
	Gui::Dispose( );
	Skybox::Dispose( );

	Window::Internal::Destroy( );
}

