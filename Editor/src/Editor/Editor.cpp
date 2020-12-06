#include "Editor.hpp"
#include "Gui\Gui.hpp"
#include <Pine\Assets\Assets.hpp>
#include <Pine/Pine.hpp>

#include "EditorEntity/EditorEntity.hpp"
#include "ProjectManager\ProjectManager.hpp"
#include "Gui\Utility\AssetIconGen\AssetIconGen.hpp"
#include "Pine/Rendering/Skybox/Skybox.hpp"
#include "RenderingHandler/RenderingHandler.hpp"

#include "Pine/Assets/Texture3D/Texture3D.hpp"

void UpdateAssetCache( );

void Editor::Setup( ) {

	Editor::ProjectManager::Setup( );
	Editor::ProjectManager::OpenProject( "Projects\\Default" );

	Pine::Assets::LoadFromDirectory( "Assets\\Editor" );

	Gui::Setup( );
	Gui::Utility::AssetIconGen::Update( );

	EditorEntity::Create( );

	UpdateAssetCache( );

	RenderingHandler::Setup( );

	Pine::SetAllowUpdates( false );
	
}
