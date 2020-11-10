#include "Editor.hpp"
#include "Gui\Gui.hpp"
#include "ProjectManager\ProjectManager.hpp"
#include <Pine\Assets\Assets.hpp>

void UpdateAssetCache( );

void Editor::Setup( ) {

	Editor::ProjectManager::Setup( );
	Editor::ProjectManager::OpenProject( "Projects\\Default" );

	Pine::Assets::LoadFromDirectory( "Assets\\Editor" );

	Editor::Gui::Setup( );

	UpdateAssetCache( );

}
