#include <iostream>
#include <Pine/Pine.hpp>
#include <Pine/Gui/Gui.hpp>
#include "Gui/Gui.hpp"
#include <Pine/Components/ModelRenderer/ModelRenderer.hpp>
#include <Pine/Entitylist/EntityList.hpp>
#include <Pine/Components/Camera/Camera.hpp>
#include <Pine/Rendering/RenderManager/RenderManager.hpp>
#include <Pine/Components/Light/Light.hpp>
#include <Pine/Assets/Blueprint/Blueprint.hpp>


#include "Pine/Assets/Level/Level.hpp"
#include "Utils/AssetPreviewGenerator/AssetPreviewGenerator.hpp"
#include "Utils/DirectoryCache/DirectoryCache.hpp"
#include "Utils/PreviewManager/PreviewManager.hpp"

Pine::Entity* entity = nullptr;

void OnRender( ) {

}

void OnRenderGui( ) {

	Editor::Gui::Run( );

}

int main( )
{
	if ( !Pine::Setup( ) ) {
		return 1;
	}

	Pine::Window::SetSize( 1280, 720 );

	Editor::Gui::Setup( );

	Pine::Assets::LoadFromDirectory( "Assets" );
	
	PreviewManager::Setup( );
	
	Editor::DirectoryCache::SetRootDirectory( "Assets" );
	Editor::DirectoryCache::Refresh( );

	Editor::AssetPreviewGenerator::Setup( );
	Editor::AssetPreviewGenerator::Generate( );
	
	// Setup rendering callbacks
	Pine::SetRenderingCallback( OnRender );
	Pine::Gui::SetGuiRenderCallback( OnRenderGui );

	Pine::SetFrameBuffer( Editor::Gui::GetViewportFrameBuffer( ) );

	Pine::Run( );

	Editor::Gui::Dispose( );
	PreviewManager::Dispose( );
	Editor::AssetPreviewGenerator::Dispose( );

	Pine::Terminate( );

	return 0;
}