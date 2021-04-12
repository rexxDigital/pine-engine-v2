#include "Windows.hpp"
#include "Editor/Gui/Widgets/Widgets.hpp"
#include "Editor/ProjectManager/ProjectManager.hpp"
#include "Pine/Entity/Entity.hpp"
#include "Pine/Rendering/RenderManager/RenderManager.hpp"
#include "Pine/Rendering/Skybox/Skybox.hpp"

using namespace Editor::Gui;

void Windows::RenderLevelSettings( )
{
	if ( !ShowLevelSettings )
		return;

	ImGui::Begin( "Level Settings", &ShowLevelSettings, 0 );

	auto renderingContext = Pine::RenderManager::GetRenderingContext( );

	const auto level = ProjectManager::GetCurrentLevel( );
	const auto newLevel = Widgets::AssetPicker( "Level", level, true, Pine::EAssetType::Level );

	if ( newLevel.valid )
	{
		auto newLevelPointer = dynamic_cast< Pine::Level* >( newLevel.asset );

		if ( newLevelPointer )
		{
			newLevelPointer->Load( );
			ProjectManager::OpenLevel( newLevelPointer );
		}
	}

	Pine::Entity* cameraEntity = nullptr;
	if ( renderingContext->m_Camera )
		cameraEntity = renderingContext->m_Camera->GetParent( );

	const auto newEntityRet = Widgets::EntityPicker( "Camera Entity", cameraEntity );
	if ( newEntityRet.valid )
	{
		auto newEntity = reinterpret_cast< Pine::Entity* >( newEntityRet.entity );

		if ( newEntity )
		{
			// Check if the entity has a camera component
			if ( auto cam = newEntity->GetComponent<Pine::Camera>( ) )
			{
				Pine::RenderManager::GetRenderingContext( )->m_Camera = cam;
			}
		}
		else
		{
			Pine::RenderManager::GetRenderingContext( )->m_Camera = nullptr;
		}
	}

	const auto newSkybox = Widgets::AssetPicker( "Skybox", reinterpret_cast< Pine::IAsset* >( Pine::Skybox::GetSkyboxCubemap( ) ), true, Pine::EAssetType::Texture3D );
	if ( newSkybox.valid )
	{
		Pine::Skybox::SetSkyboxCubemap( reinterpret_cast< Pine::Texture3D* >( newSkybox.asset ) );
	}

	ImGui::End( );
}
