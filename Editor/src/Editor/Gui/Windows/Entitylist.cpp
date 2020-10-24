#include "Windows.hpp"
#include "../Gui.hpp"

#include <Pine/Pine.hpp>
#include <Pine/Entitylist/EntityList.hpp>

namespace {

	// HACK: To fix context menus, since IsWindowHovered is out of control
	bool g_DidOpenContextMenu = false;

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
				ImGui::OpenPopup( "EntityContextMenu" );
				g_DidOpenContextMenu = true;
			}
		}
		else {
			auto flags = ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_SpanFullWidth;

			if ( isSelectedEntity )
				flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;

			if ( ImGui::TreeNodeEx( renderText.c_str( ), flags ) ) {
				if ( ImGui::IsItemClicked( ImGuiMouseButton_::ImGuiMouseButton_Right ) ) {
					SelectEntity( e );
					ImGui::OpenPopup( "EntityContextMenu" );
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
					ImGui::OpenPopup( "EntityContextMenu" );
					g_DidOpenContextMenu = true;
				}
			}
		}
	}
}

void Editor::Gui::Windows::RenderEntitylist( ) {
	if ( !ShowEntitylist )
		return;

	ImGui::Begin( "Entity list", &ShowEntitylist );

	auto& entities = Pine::EntityList::GetEntities( );

	for ( auto& entity : entities ) {

		// Since we render children for each parent entity instead.
		if ( entity->GetParent( ) != nullptr ) {
			continue;
		}

		RenderEntity( entity );
	}

	if ( ImGui::IsMouseClicked( ImGuiMouseButton_::ImGuiMouseButton_Left ) && ImGui::IsWindowHovered( ) ) {
		Editor::Gui::Globals::SelectedEntityPtrs.clear( );
	}

	if ( ImGui::IsMouseClicked( ImGuiMouseButton_::ImGuiMouseButton_Right ) && ImGui::IsWindowHovered( ) && !g_DidOpenContextMenu ) {
		Editor::Gui::Globals::SelectedEntityPtrs.clear( );
		ImGui::OpenPopup( "EntityContextMenu" );
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

		if ( ImGui::MenuItem( "Create a child", nullptr, false, isTargetingEntity ) ) {
			e->CreateChild( );
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
