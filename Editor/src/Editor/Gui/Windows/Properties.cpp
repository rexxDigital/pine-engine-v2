#include "Windows.hpp"

#include "../Gui.hpp"

#include <Pine\Entity\Entity.hpp>
#include <Pine\Assets\IAsset\IAsset.hpp>
#include "../Utility/ComponentPropertiesRenderer/ComponentPropertiesRenderer.hpp"

namespace {

	void DisplayEntityProperties( Pine::Entity* e ) {
		ImGui::Text( "Entity" );

		static char nameBuffer[ 64 ];
		bool enabled = e->GetActive( );

		if ( ImGui::Checkbox( std::string( "##Active" + std::to_string( e->GetId( ) ) ).c_str( ), &enabled ) ) {
			e->SetActive( enabled );
		}

		ImGui::SameLine( );

		strcpy_s( nameBuffer, e->GetName( ).c_str( ) );

		if ( ImGui::InputText( std::string( "##Name" + std::to_string( e->GetId( ) ) ).c_str( ), nameBuffer, 64 ) ) {
			e->SetName( nameBuffer );
		}

		ImGui::Spacing( );
		ImGui::Separator( );
		ImGui::Spacing( );

		int index = 0;
		for ( auto component : e->GetComponents( ) ) {
			if ( ImGui::CollapsingHeader( std::string( std::string( Pine::SComponentNames[ static_cast< int >( component->GetType( ) ) ] ) + "##" + std::to_string( index ) ).c_str( ), ImGuiTreeNodeFlags_DefaultOpen ) ) {
				bool active = component->GetActive( );

				ImGui::Spacing( );

				if ( ImGui::Checkbox( std::string( "Active##" + std::to_string( index ) ).c_str( ), &active ) ) {
					component->SetActive( active );
				}

				ImGui::SameLine( );

				if ( ImGui::Button( std::string( "Remove##" + std::to_string( index ) ).c_str( ) ) ) {

				}

				ImGui::Spacing( );
				ImGui::Separator( );

				Editor::Gui::Utility::ComponentPropertiesRenderer::RenderComponentProperties( component );
			}

			index++;
		}

	}

	void DisplayAssetProperties( Pine::IAsset* a ) {

	}

}

void Editor::Gui::Windows::RenderProperties( ) {

	if ( !ShowProperties ) {
		return;
	}

	ImGui::Begin( "Properties", &ShowProperties, 0 );

	if ( Gui::Globals::SelectedItem == SelectedItemType::Entity && !Gui::Globals::SelectedEntityPtrs.empty( ) ) {
		DisplayEntityProperties( Gui::Globals::SelectedEntityPtrs[ 0 ] );
	}

	if ( Gui::Globals::SelectedItem == SelectedItemType::Asset && !Gui::Globals::SelectedAssetPtrs.empty( ) ) {
		DisplayAssetProperties( Gui::Globals::SelectedAssetPtrs[ 0 ] );
	}

	ImGui::End( );

}