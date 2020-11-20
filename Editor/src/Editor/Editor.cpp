#include "Editor.hpp"
#include "Gui\Gui.hpp"
#include <Pine\Assets\Assets.hpp>

#include "ProjectManager\ProjectManager.hpp"
#include "Gui\Utility\AssetIconGen\AssetIconGen.hpp"
#include "RenderingHandler/RenderingHandler.hpp"

void UpdateAssetCache( );

void Editor::Setup( ) {

	Editor::ProjectManager::Setup( );
	Editor::ProjectManager::OpenProject( "Projects\\Default" );

	Pine::Assets::LoadFromDirectory( "Assets\\Editor" );

	Gui::Setup( );
	Gui::Utility::AssetIconGen::Update( );

	UpdateAssetCache( );

	RenderingHandler::Setup();

}
