#include <iostream>
#include <Windows.h>

#include "Pine/Pine.hpp"
#include "Pine/Components/Camera/Camera.hpp"
#include "Pine/Components/ModelRenderer/ModelRenderer.hpp"
#include "Pine/Entitylist/EntityList.hpp"
#include "Pine/Rendering/RenderManager/RenderManager.hpp"
#include "PlayerController/PlayerController.hpp"

bool InitializeGame( )
{
	Pine::Window::SetTitle( "Sample game" );
	Pine::Window::SetSize( 1280, 720 );

	if ( Pine::Assets::LoadFromDirectory( "Assets\\Game" ) == 0 )
	{
		return false;
	}

	// Setup our player
	const auto playerEntity = Pine::EntityList::CreateEntity( "Player" );

	playerEntity->AddComponent( Pine::EComponentType::Camera );
	playerEntity->AddComponent( new PlayerController( ) );

	// Setup our terrain
	const auto groundEntity = Pine::EntityList::CreateEntity( "Ground" );

	groundEntity->AddComponent( Pine::EComponentType::ModelRenderer );
	groundEntity->GetComponent<Pine::ModelRenderer>( )->SetTargetModel( Pine::Assets::GetAsset<Pine::Model>( "Assets\\Game\\cube.fbx" ) );

	Pine::RenderManager::GetRenderingContext( )->m_Camera = playerEntity->GetComponent<Pine::Camera>( );
	Pine::RenderManager::GetRenderingContext( )->m_Width = 1280;
	Pine::RenderManager::GetRenderingContext( )->m_Height = 720;

	return true;
}

int main( ) {
	if ( !Pine::Setup( ) )
	{
		MessageBoxA( nullptr, "Failed to setup game engine.", nullptr, 0 );
		return 1;
	}

	if ( !InitializeGame( ) )
	{
		MessageBoxA( nullptr, "Failed to setup game.", nullptr, 0 );
		return 2;
	}

	Pine::Run( );

	Pine::Terminate( );

	return 0;
}