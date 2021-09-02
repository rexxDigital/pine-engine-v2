#include "Pine.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>
#include "Rendering/RenderManager/RenderManager.hpp"
#include "Rendering/UniformBuffers/UniformBuffers.hpp"
#include "Entitylist/EntityList.hpp"
#include "Gui/Gui.hpp"
#include "Rendering/Skybox/Skybox.hpp"
#include "Assets/Texture3D/Texture3D.hpp"
#include "Components/Components.hpp"
#include "Core/Log/Log.hpp"
#include "Input/Input.hpp"
#include "Rendering/PostProcessing/PostProcessing.hpp"
#include "ScriptManager/ScriptManager.hpp"

namespace
{
	constexpr int TickRate = 144;

	bool g_StopUpdateThread = false;
	bool g_AllowUpdates = false;

	double GetTimeAsDouble( ) {
		using namespace std::chrono;
		using SecondsFP = std::chrono::duration<double>;
		return duration_cast< SecondsFP >( high_resolution_clock::now( ).time_since_epoch( ) ).count( );
	}

	void UpdateThread( )
	{
		double lastTime = GetTimeAsDouble( );

		while ( !g_StopUpdateThread )
		{
			const double currentTime = GetTimeAsDouble( );
			const float deltaTime = static_cast< float >( currentTime - lastTime );

			if ( g_AllowUpdates )
				Pine::EntityList::RunOnUpdate( deltaTime );

			lastTime = currentTime;
			std::this_thread::sleep_for( std::chrono::milliseconds( static_cast< int >( 1000.f / TickRate ) ) );
		}
	}

}

void Pine::SetAllowUpdates( bool value )
{
	g_AllowUpdates = value;
}

bool Pine::IsAllowingUpdates( )
{
	return g_AllowUpdates;
}

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
		// Since we already printed out an error message in the function, no need to do it again.
		return false;
	}

	glewExperimental = true;

	if ( glewInit( ) != GLEW_OK )
	{
		Log::Fatal( "Failed to initialize GLEW." );
		return false;
	}

	const auto gpuRenderer = glGetString( GL_RENDERER );

	Log::Message( "Using GPU: " + std::string( reinterpret_cast< const char* >( gpuRenderer ) ) );

	// Setup some core stuff first.
	Assets::Setup( );
	UniformBuffers::Setup( );

	// We want to load engine shaders first because some other engine assets needs the shaders to be ready first.
	Log::Message( "Loading engine shaders..." );

	if ( Assets::LoadFromDirectory( "Assets\\Engine\\Shaders", true ) == 0 ) {
		Log::Fatal( "Failed to load engine shaders, required engine files are missing from the executable directory." );
		return false;
	}

	Log::Message( "Loading engine assets..." );

	if ( Assets::LoadFromDirectory( "Assets\\Engine", true ) == 0 ) {
		Log::Fatal( "Failed to load engine assets, required engine files are missing from the executable directory." );
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

	if ( g_AllowUpdates )
		EntityList::RunOnSetup( );

	std::thread t( UpdateThread );

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

	g_StopUpdateThread = true;

	t.join( );
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

