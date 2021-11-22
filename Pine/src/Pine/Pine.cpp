#include "Pine.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>

#include "Core/Interfaces/Interfaces.hpp"

#include "Rendering/RenderManager/RenderManager.hpp"
#include "Rendering/UniformBuffers/UniformBuffers.hpp"
#include "Entitylist/EntityList.hpp"
#include "Gui/Gui.hpp"
#include "Rendering/Skybox/Skybox.hpp"
#include "Assets/Texture3D/Texture3D.hpp"
#include "Components/Components.hpp"
#include "Core/Log/Log.hpp"
#include "Input/Input.hpp"
#include "PhysicsManager/PhysicsManager.hpp"
#include "Rendering/PostProcessing/PostProcessing.hpp"

namespace
{
	Pine::PineInstance g_PineInstance;

	constexpr int TickRate = 144;

	bool g_StopUpdateThread = false;
	bool g_AllowUpdates = true;

	float g_WindowTime = 0.0f;

	double GetTimeAsDouble( )
	{
		using namespace std::chrono;
		using SecondsFP = duration<double>;
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
				Pine::EntityList->RunOnUpdate( deltaTime );

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

float Pine::GetTime( )
{
	return g_WindowTime;
}

Pine::PineInstance* Pine::GetPineInstance( )
{
	return &g_PineInstance;
}

bool Pine::Setup( )
{
	// Since Setup was called we assume this is the host instance, create all interfaces and store em.
	g_PineInstance = CreateInstance( );

	// Setup all global interface pointers to the instance we just created.
	UseInstance( &g_PineInstance );

	Log->Message( "Setting up Pine..." );

	if ( !glfwInit( ) )
	{
		Log->Fatal( "Failed to initialize GLFW." );
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
		Log->Fatal( "Failed to initialize GLEW." );
		return false;
	}

	const auto gpuRenderer = glGetString( GL_RENDERER );

	Log->Message( "Using GPU: " + std::string( reinterpret_cast< const char* >( gpuRenderer ) ) );

	// Setup some core stuff first.
	Assets->Setup( );
	UniformBuffers::Setup( );

	// We want to load engine shaders first because some other engine assets needs the shaders to be ready first.
	Log->Message( "Loading engine shaders..." );

	if ( Assets->LoadFromDirectory( "Assets\\Engine\\Shaders", true ) == 0 ) 
	{
		Log->Fatal( "Failed to load engine shaders, required engine files are missing from the executable directory." );
		return false;
	}

	Log->Message( "Loading engine assets..." );

	if ( Assets->LoadFromDirectory( "Assets\\Engine", true ) == 0 ) 
	{
		Log->Fatal( "Failed to load engine assets, required engine files are missing from the executable directory." );
		return false;
	}

	RenderManager->SetRenderingContext( CreateDefaultRenderingContext( ) );

	PhysicsManager->Setup( );
	Renderer3D->Setup( );
	Skybox->Setup( );
	Gui->Setup( );
	RenderManager->Setup( );
	PostProcessing->Setup( );
	Components->Setup( );
	EntityList->Setup( );

	Skybox->SetSkyboxCubemap( Assets->GetAsset<Texture3D>( "Assets\\Engine\\Skyboxes\\DefaultSkybox.cmap" ) );

	Log->Message( "Pine was successfully initialized!" );

	return true;
}

void Pine::Run( )
{
	Window::Show( );
	Window::UpdateCachedSize( );

	if ( g_AllowUpdates )
		EntityList->RunOnSetup( );

	std::thread t( UpdateThread );

	const auto window = Window::Internal::GetWindowPointer( );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );

	double lastFrameTime = 0;

	while ( !glfwWindowShouldClose( window ) )
	{
		// Calculate delta time
		g_WindowTime = glfwGetTime( );
		const double deltaTime = g_WindowTime - lastFrameTime;
		lastFrameTime = g_WindowTime;

		Input->Update( );
		PhysicsManager->Update( deltaTime );

		RenderManager->Render( );
		Gui->Render( );

		glfwSwapBuffers( window );
		glfwPollEvents( );
	}

	g_StopUpdateThread = true;

	t.join( );
}

void Pine::Terminate( )
{
	Assets->Dispose( );
	UniformBuffers::Dispose( );
	Renderer3D->Dispose( );
	Gui->Dispose( );
	Skybox->Dispose( );
	PostProcessing->Dispose( );
	Components->Dispose( );
	PhysicsManager->Dispose( );

	Window::Internal::Destroy( );
}

