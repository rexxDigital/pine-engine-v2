#include "Windows.hpp"
#include "../Gui.hpp"

#include <Pine/Pine.hpp>
#include <Pine/Entitylist/EntityList.hpp>

#include "Editor/EditorEntity/EditorEntity.hpp"
#include "Editor/Gui/Utility/HotkeyManager/HotkeyManager.hpp"
#include "Pine/Assets/Blueprint/Blueprint.hpp"

namespace {

	// HACK: To fix context menus, since IsWindowHovered is out of control
	bool g_DidOpenContextMenu = false;
	bool g_OpenContextMenu = false;

	// Since we might support selecting multiple entities in the future.
	bool IsSelectedEntity( Pine::Entity* e ) {
		for ( auto entity : Editor::Gui::Globals::SelectedEntityPtrs ) {
			if ( entity == e ) {
				return true;
			}
		}

		return false;
	}

	void SelectEntity( Pine::Entity* e ) {
		if ( !ImGui::GetIO( ).KeyCtrl )
			Editor::Gui::Globals::SelectedEntityPtrs.clear( );

		Editor::Gui::Globals::SelectedEntityPtrs.push_back( e );
		Editor::Gui::Globals::SelectedAssetPtrs.clear( );

		Editor::Gui::Globals::SelectedItem = Editor::Gui::SelectedItemType::Entity;
	}

	void RenderEntity( Pine::Entity* e ) {
		const std::string renderText = e->GetName( ) + "##" + std::to_string( e->GetId( ) );
		const bool isSelectedEntity = IsSelectedEntity( e );

		if ( e->GetChildren( ).empty( ) ) {
			if ( ImGui::Selectable( renderText.c_str( ), isSelectedEntity, 0 ) ) {
				SelectEntity( e );
			}

			if ( ImGui::IsItemClicked( ImGuiMouseButton_::ImGuiMouseButton_Right ) ) {
				SelectEntity( e );

				g_OpenContextMenu = true;
				g_DidOpenContextMenu = true;
			}

			if ( ImGui::BeginDragDropTarget( ) ) {
				if ( const auto payload = ImGui::AcceptDragDropPayload( "Entity", 0 ) ) {
					const auto entity = *reinterpret_cast< Pine::Entity** >( payload->Data );
					
					e->AddChild( entity );
				}

				ImGui::EndDragDropTarget( );
			}

			if ( ImGui::BeginDragDropSource( ImGuiDragDropFlags_::ImGuiDragDropFlags_None ) ) {
				ImGui::SetDragDropPayload( "Entity", &e, sizeof( Pine::IAsset* ), 0 );

				ImGui::Text( e->GetName( ).c_str( ) );

				ImGui::EndDragDropSource( );
			}
		}
		else {
			auto flags = ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_SpanFullWidth;

			if ( isSelectedEntity )
				flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;

			if ( ImGui::TreeNodeEx( renderText.c_str( ), flags ) ) {
				if ( ImGui::IsItemClicked( ImGuiMouseButton_::ImGuiMouseButton_Right ) ) {
					SelectEntity( e );

					g_OpenContextMenu = true;
					g_DidOpenContextMenu = true;
				}

				if ( ImGui::IsItemClicked( ) ) {
					SelectEntity( e );
				}

				auto& children = e->GetChildren( );

				for ( auto& child : children ) {
					RenderEntity( child );
				}

				ImGui::TreePop( );
			}
			else {
				if ( ImGui::IsItemClicked( ) ) {
					SelectEntity( e );
				}

				if ( ImGui::IsItemClicked( ImGuiMouseButton_::ImGuiMouseButton_Right ) ) {
					SelectEntity( e );

					g_OpenContextMenu = true;
					g_DidOpenContextMenu = true;
				}
			}
			
			if ( ImGui::BeginDragDropTarget( ) ) {
				if ( const auto payload = ImGui::AcceptDragDropPayload( "Entity", 0 ) ) {
					const auto entity = *reinterpret_cast< Pine::Entity** >( payload->Data );

					e->AddChild( entity );
				}

				ImGui::EndDragDropTarget( );
			}

			if ( ImGui::BeginDragDropSource( ImGuiDragDropFlags_::ImGuiDragDropFlags_None ) ) {
				ImGui::SetDragDropPayload( "Entity", &e, sizeof( Pine::IAsset* ), 0 );

				ImGui::Text( e->GetName( ).c_str( ) );

				ImGui::EndDragDropSource( );
			}
		}
	}

	void EntityMoveSeparator( int entityIndex ) {
		ImGui::Separator( );
		
		if ( ImGui::BeginDragDropTarget( ) ) {
			if ( const auto payload = ImGui::AcceptDragDropPayload( "Entity", 0 ) ) {
				const auto entity = *reinterpret_cast< Pine::Entity** >( payload->Data );

				if ( entityIndex >= Pine::EntityList::GetEntities( ).size( ) )
					entityIndex = Pine::EntityList::GetEntities( ).size( ) - 1;
				
				Pine::EntityList::MoveEntity( entity, entityIndex );
			}

			ImGui::EndDragDropTarget( );
		}		
	}
	
}

void Editor::Gui::Windows::RenderEntitylist( ) {
	if ( !ShowEntitylist )
		return;

	ImGui::Begin( "Entity list", &ShowEntitylist );

	auto& entities = Pine::EntityList::GetEntities( );

	bool isDragDroppingEntity = false;

	if ( auto payload = ImGui::GetDragDropPayload( ) )
	{
		if ( std::string( payload->DataType ).find( "Entity" ) != std::string::npos )
		{
			isDragDroppingEntity = true;
		}
	}

	if ( isDragDroppingEntity )
		EntityMoveSeparator( 0 );

	int entityRenderIndex = 0;
	
	for ( auto& entity : entities ) {
		entityRenderIndex++;
		
		// Since we render children for each parent entity instead.
		if ( entity.GetParent( ) != nullptr ) {
			continue;
		}

		if ( &entity == EditorEntity::GetEntity( ) ) {
			continue;
		}

		RenderEntity( &entity );

		if ( isDragDroppingEntity )
			EntityMoveSeparator( entityRenderIndex );
	}

	if ( ImGui::IsMouseClicked( ImGuiMouseButton_::ImGuiMouseButton_Left ) && ImGui::IsWindowHovered( ) ) {
		Editor::Gui::Globals::SelectedEntityPtrs.clear( );
		Editor::Gui::Globals::SelectedAssetPtrs.clear( );
	}

	if ( g_OpenContextMenu )
	{
		ImGui::OpenPopup( "EntityContextMenu" );

		g_OpenContextMenu = false;
	}

	if ( ImGui::IsMouseClicked( ImGuiMouseButton_::ImGuiMouseButton_Right ) && ImGui::IsWindowHovered( ) && !g_DidOpenContextMenu ) {
		Editor::Gui::Globals::SelectedEntityPtrs.clear( );
		Editor::Gui::Globals::SelectedAssetPtrs.clear( );
		ImGui::OpenPopup( "EntityContextMenu" );
	}

	if ( Editor::Gui::Globals::SelectedEntityPtrs.size( ) == 1 ) {
		Pine::Entity* e = Editor::Gui::Globals::SelectedEntityPtrs[ 0 ];

		if ( HotkeyManager::GetHotkeyPressed( Hotkeys::RemoveEntityKey ) ) {
			Pine::EntityList::DeleteEntity( e );
			Editor::Gui::Globals::SelectedEntityPtrs.clear( );
		}

		if ( HotkeyManager::GetHotkeyPressed( Hotkeys::DuplicateEntity ) ) {
			Pine::Blueprint blueprint;

			blueprint.CreateFromEntity( e );

			const auto newEntity = blueprint.SpawnEntity( );

			blueprint.Dispose( );

			// We'll have to manually assign the parent as the blueprint won't be able to do that, as it wouldn't make any sense.
			if ( e->GetParent(  ) != nullptr ) {
				e->GetParent( )->AddChild( newEntity );
			}
		}
	}

	if ( ImGui::BeginPopup( "EntityContextMenu", 0 ) ) {
		const bool isTargetingEntity = Editor::Gui::Globals::SelectedEntityPtrs.size( ) == 1;

		g_DidOpenContextMenu = false;

		Pine::Entity* e = nullptr;
		if ( isTargetingEntity )
			e = Editor::Gui::Globals::SelectedEntityPtrs[ 0 ];


		if ( ImGui::MenuItem( "Remove", nullptr, false, isTargetingEntity ) ) {
			Pine::EntityList::DeleteEntity( e );
			Editor::Gui::Globals::SelectedEntityPtrs.clear( );
			ImGui::CloseCurrentPopup( );
		}

		if ( ImGui::MenuItem( "Duplicate", nullptr, false, isTargetingEntity ) ) {
			Pine::Blueprint blueprint;

			blueprint.CreateFromEntity( e );
			blueprint.SpawnEntity( );

			blueprint.Dispose( );

			ImGui::CloseCurrentPopup( );
		}

		if ( ImGui::MenuItem( "Create a child", nullptr, false, isTargetingEntity ) ) {
			e->CreateChild( );
			ImGui::CloseCurrentPopup( );
		}

		if ( ImGui::MenuItem( "Unlink from parent", nullptr, false, isTargetingEntity && e->GetParent(  ) != nullptr ) ) {
			e->GetParent( )->RemoveChild( e );
			
			ImGui::CloseCurrentPopup( );
		}

		ImGui::Separator( );

		if ( ImGui::MenuItem( "Create entity" ) ) {
			Pine::EntityList::CreateEntity( );
			ImGui::CloseCurrentPopup( );
		}

		ImGui::EndPopup( );
	}

	ImGui::End( );

}
