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
#include "Pine/Core/Log/Log.hpp"

void UpdateAssetCache( );
void InitializePicPanel( );

void Editor::Setup( ) {

	Pine::Log->Message( "Setting up editor..." );
	
	ProjectManager::Setup( );
	ProjectManager::OpenProject( "Projects\\Debug" );

	Pine::Log->Message( "Loading editor assets..." );
	
	Pine::Assets->LoadFromDirectory( "Assets\\Editor" );

	Gui::Setup( );
	Gui::Utility::AssetIconGen::Update( );

	InitializePicPanel( );

	EditorEntity::Create( );

	UpdateAssetCache( );

	RenderingHandler::Setup( );

	Pine::SetAllowUpdates( false );

	Pine::Log->Message( "Ready." );
	
}
