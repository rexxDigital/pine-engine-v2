#include "Windows.hpp"
#include "../Gui.hpp"

#include <Pine/Pine.hpp>
#include <Pine/Entitylist/EntityList.hpp>

namespace {

	// HACK: To detect if I press anything outside the entities, need to replace this.
	bool g_PressedEntity = false;

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
		}
		else {
			auto flags = ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen;

			if ( isSelectedEntity )
				flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;

			if ( ImGui::TreeNodeEx( renderText.c_str( ), flags ) ) {
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

	if ( ImGui::IsMouseDown( 0 ) && ImGui::IsWindowHovered( ) ) {
		Editor::Gui::Globals::SelectedEntityPtrs.clear( );
	}
		
	ImGui::End( );

}
