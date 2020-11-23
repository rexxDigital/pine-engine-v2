#include "Windows.hpp"

#include "../Gui.hpp"

#include <Pine\Entity\Entity.hpp>
#include <Pine\Assets\IAsset\IAsset.hpp>
#include "../Utility/ComponentPropertiesRenderer/ComponentPropertiesRenderer.hpp"
#include "../Widgets/Widgets.hpp"
#include <Pine/Components/Components.hpp>
#include "../Utility/AssetIconGen/AssetIconGen.hpp"
#include "../Utility/AssetPropertiesRenderer/AssetPropertiesRenderer.hpp"

namespace {

	void DisplayEntityProperties( Pine::Entity* e ) {
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

				if ( index == 0 ) {
					Editor::Gui::Widgets::PushDisabled( );
				}

				if ( ImGui::Checkbox( std::string( "Active##" + std::to_string( index ) ).c_str( ), &active ) ) {
					component->SetActive( active );
				}

				ImGui::SameLine( );

				if ( ImGui::Button( std::string( "Remove##" + std::to_string( index ) ).c_str( ) ) ) {
					e->RemoveComponent( index );
					break;
				}

				if ( index == 0 ) {
					Editor::Gui::Widgets::PopDisabled( );
				}

				ImGui::Spacing( );
				ImGui::Separator( );
				ImGui::Spacing( );

				Editor::Gui::Utility::ComponentPropertiesRenderer::RenderComponentProperties( component );
			}

			index++;
		}

		ImGui::Spacing( );
		ImGui::Separator( );
		ImGui::Spacing( );

		if ( ImGui::Button( "Add new component...", ImVec2( -1.f, 35.f ) ) ) {
			ImGui::OpenPopup( "AddNewComponent" );
		}

	}

	void DisplayAssetProperties( Pine::IAsset* a ) {

		ImGui::Image( reinterpret_cast< ImTextureID >( Editor::Gui::Utility::AssetIconGen::GetAssetIcon( a->GetPath( ).string( ) )->GetId( ) ), ImVec2( 64.f, 64.f ) );

		ImGui::SameLine( );

		ImGui::BeginChild( "##AssetPropertiesChild", ImVec2( -1.f, 65.f ), false, 0 );

		ImGui::Text( "%s", a->GetFileName( ).c_str( ) );
		ImGui::Text( "%s", a->GetPath( ).string( ).c_str( ) );
		ImGui::Text( "%s", Pine::SAssetType[ static_cast< int >( a->GetType( ) ) ] );

		ImGui::EndChild( );

		ImGui::Spacing( );
		ImGui::Separator( );
		ImGui::Spacing( );

		Editor::Gui::Utility::AssetPropertiesRenderer::RenderAssetProperties( a );

	}

	void DisplayAddComponentPopup( ) {
		static char buff[ 64 ];
		static bool initialize = true;
		static int selectedItem = 0;

		static std::vector<Pine::EComponentType> components;

		static std::vector<const char*> displayComponents;
		static std::vector<Pine::EComponentType> displayComponentsType;

		if ( ImGui::BeginPopup( "AddNewComponent", 0 ) ) {
			if ( initialize ) {
				strcpy_s( buff, "\0" );

				components.clear( );
				displayComponents.clear( );
				displayComponentsType.clear( );

				for ( int i = 0; i < Pine::Components::GetComponentCount( ); i++ ) {
					components.push_back( static_cast< Pine::EComponentType >( i ) );

					if ( i > 1 ) {
						displayComponents.push_back( Pine::Components::GetComponentName( components[ i ] ) );
						displayComponentsType.push_back( static_cast< Pine::EComponentType >( i ) );
					}
				}

				ImGui::SetKeyboardFocusHere( );

				selectedItem = 0;

				initialize = false;
			}

			ImGui::SetNextItemWidth( -1.f );

			if ( ImGui::InputText( "##SearchBuffer", buff, 64 ) ) {
				displayComponents.clear( );
				displayComponentsType.clear( );

				for ( int i = 2; i < components.size( ); i++ ) {
					// To search case insensitive I have to this retarded stuff.
					char componentNameLowercase[ 64 ];
					char searchBufferLowercase[ 64 ];

					auto componentName = Pine::Components::GetComponentName( components[ i ] );

					for ( int j = 0; j < strlen( componentName ) + 1; j++ ) {
						componentNameLowercase[ j ] = tolower( componentName[ j ] );
					}

					for ( int j = 0; j < strlen( buff ) + 1; j++ ) {
						searchBufferLowercase[ j ] = tolower( buff[ j ] );
					}

					if ( strstr( componentNameLowercase, searchBufferLowercase ) != 0 ) {
						displayComponents.push_back( Pine::Components::GetComponentName( components[ i ] ) );
						displayComponentsType.push_back( components[ i ] );
					}
				}

				selectedItem = 0;
			}

			ImGui::ListBox( "##Components", &selectedItem, displayComponents.data( ), displayComponents.size( ) );

			if ( ImGui::Button( "OK" ) || ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_::ImGuiKey_Enter ) ) ) {
				if ( Editor::Gui::Globals::SelectedItem == Editor::Gui::SelectedItemType::Entity && !Editor::Gui::Globals::SelectedEntityPtrs.empty( ) ) {
					auto comp = Pine::Components::CreateComponent( displayComponentsType[ selectedItem ] );
					if ( comp != nullptr )
						Editor::Gui::Globals::SelectedEntityPtrs[ 0 ]->AddComponent( comp );
				}

				ImGui::CloseCurrentPopup( );
			}

			ImGui::SameLine( );

			if ( ImGui::Button( "Cancel" ) || ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_::ImGuiKey_Escape ) ) ) {
				ImGui::CloseCurrentPopup( );
			}

			ImGui::EndPopup( );
		}
		else {
			initialize = true;
		}
	}

}

void Editor::Gui::Windows::RenderProperties( ) {

	if ( !ShowProperties ) {
		return;
	}

	ImGui::Begin( "Properties", &ShowProperties, 0 );

	if ( !Gui::Globals::SelectedEntityPtrs.empty( ) ) {
		DisplayEntityProperties( Gui::Globals::SelectedEntityPtrs[ 0 ] );
	}

	if ( !Gui::Globals::SelectedAssetPtrs.empty( ) ) {
		DisplayAssetProperties( Gui::Globals::SelectedAssetPtrs[ 0 ] );
	}

	DisplayAddComponentPopup( );

	ImGui::End( );

}