#include <Pine/Pine.hpp>

#include "Pine/Entitylist/EntityList.hpp"
#include "Pine/Rendering/RenderManager/RenderManager.hpp"

#include "Controllers/PlayerController/PlayerController.hpp"
#include "Pine/Components/ModelRenderer/ModelRenderer.hpp"

void CreatePlayer( )
{
	const auto player = Pine::EntityList->CreateEntity( "Player" );

	//player->GetTransform( )->Position.y = 4.f;

	player->AddComponent( Pine::ComponentType::Camera );
	//player->AddComponent( new PlayerController( ) );

	Pine::RenderManager->GetRenderingContext( )->m_Camera = player->GetComponent<Pine::Camera>( );
}

void CreateWorld( )
{
	// The sun
	const auto light = Pine::EntityList->CreateEntity( "Light" );

	light->AddComponent( Pine::ComponentType::Light );
	light->GetTransform( )->Position.y = 500.f;
	light->GetTransform( )->Position.x = 100.f;

	// The floor
	const auto floor = Pine::EntityList->CreateEntity( "Floor" );

	floor->AddComponent( Pine::ComponentType::ModelRenderer );
	floor->GetComponent<Pine::ModelRenderer>( )->SetModel( Pine::Assets->GetAsset<Pine::Model>( "Assets\\Engine\\Primitive Shapes\\cube.fbx" ) );

	floor->GetTransform( )->Position.z = -30.f;
}

int main( )
{
	if ( !Pine::Setup( ) )
	{
		return 1;
	}

	Pine::Window::SetTitle( "Sample Game" );
	Pine::Window::SetSize( 1280, 720 );

//	Pine::Input->SetCursorAutoCenter( true );

	//CreatePlayer( );
	//CreateWorld( );

	Pine::Run( );

	Pine::Terminate( );
}

