#include "Windows.hpp"
#include "Editor/Gui/Widgets/Widgets.hpp"
#include "Editor/ProjectManager/ProjectManager.hpp"
#include "Pine/Entity/Entity.hpp"
#include "Pine/Rendering/RenderManager/RenderManager.hpp"

using namespace Editor::Gui;

void Windows::RenderLevelSettings( )
{
	if ( !ShowLevelSettings )
		return;
	
	ImGui::Begin( "Level Settings", &ShowLevelSettings, 0 );

	auto renderingContext = Pine::RenderManager::GetRenderingContext( );
	
	auto level = ProjectManager::GetCurrentLevel( );
	if ( auto newLevel = dynamic_cast< Pine::Level* >( Widgets::AssetPicker( "Level", level, true, Pine::EAssetType::Level ) ) )
	{
		newLevel->Load( );
		
		ProjectManager::OpenLevel( newLevel );
	}

	Pine::Entity* cameraEntity = nullptr;
	if ( renderingContext->m_Camera )
		cameraEntity = renderingContext->m_Camera->GetParent( );

	if ( auto newEntity = Widgets::EntityPicker( "Camera Entity", cameraEntity ) )
	{
		// Check if the entity has a camera component
		if ( auto cam = newEntity->GetComponent<Pine::Camera>( ) )
		{
			Pine::RenderManager::GetRenderingContext( )->m_Camera = cam;
		}
	}
	
	ImGui::End( );
}
