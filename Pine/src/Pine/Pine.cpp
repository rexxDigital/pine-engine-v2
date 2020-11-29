#include "Pine.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Rendering/RenderManager/RenderManager.hpp"
#include "Rendering/UniformBuffers/UniformBuffers.hpp"
#include "Entitylist/EntityList.hpp"
#include "Gui/Gui.hpp"
#include "Rendering/Skybox/Skybox.hpp"
#include "Assets/Texture3D/Texture3D.hpp"
#include "Components/Components.hpp"
#include "Input/Input.hpp"
#include "Rendering/PostProcessing/PostProcessing.hpp"
#include "ScriptManager/ScriptManager.hpp"

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

	if ( Assets::LoadFromDirectory( "Assets\\Engine\\Shaders", true ) == 0 ) {
		Log::Fatal( "Failed to load engine shaders." );
		return false;
	}

	Log::Message( "Loading engine assets..." );

	if ( Assets::LoadFromDirectory( "Assets\\Engine", true ) == 0 ) {
		Log::Fatal( "Failed to load engine assets." );
		return false;
	}

	RenderManager::SetRenderingContext( CreateDefaultRenderingContext( ) );

	Renderer3D::Setup( );
	Skybox::Setup( );
	Gui::Setup( );
	RenderManager::Setup( );
	PostProcessing::Setup( );
	ScriptingManager::Setup( );

	Components::Internal::RegisterPineComponents( );
	Skybox::SetSkyboxCubemap( Assets::GetAsset<Texture3D>( "Assets\\Engine\\Skyboxes\\DefaultSkybox.cmap" ) );
	
	Log::Message( "Pine was successfully initialized!" );

	return true;
}

void Pine::Run( )
{
	Window::Show( );
	Window::UpdateCachedSize( );

	ScriptingManager::CompileScripts( );
	
	EntityList::RunOnSetup( );

	const auto window = Window::Internal::GetWindowPointer( );

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );

	glCullFace( GL_BACK );

	while ( !glfwWindowShouldClose( window ) )
	{
		RenderManager::Run( );
		Input::Update( );
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
	PostProcessing::Dispose( );
	ScriptingManager::Dispose( );
	
	Window::Internal::Destroy( );
}

