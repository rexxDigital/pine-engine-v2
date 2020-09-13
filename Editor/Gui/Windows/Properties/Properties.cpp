#include "Properties.hpp"
#include "ImGui/imgui.h"
#include "../../Gui.hpp"
#include "../../ComponentRenderer/ComponentRenderer.hpp"
#include <Pine/Entity/Entity.hpp>

#include "../../../Utils/AssetPreviewGenerator/AssetPreviewGenerator.hpp"
#include "../../AssetRenderer/AssetRenderer.hpp"

namespace {

	void DisplayEntity( Pine::Entity* e ) {

		bool active = e->GetActive( );
		if ( ImGui::Checkbox( "##Active", &active ) ) {
			e->SetActive( active );
		}

		ImGui::SameLine( );
		
		char buff[ 64 ];
		strcpy_s( buff, e->GetName( ).c_str( ) );
		if ( ImGui::InputText( "##EntityName", buff, 64 ) ) {
			e->SetName( buff );
		}
		
		ImGui::Text( "Entity index: %d", e->GetEntityIndex(  ) );
		ImGui::Text( "%d component(s)", e->GetComponents( ).size( ) );
		ImGui::Text( "Is temporary marked: %d", e->IsTemporary(  ) );

		if ( ImGui::IsItemHovered( ) )
			ImGui::SetTooltip( "Used for entities that won't get saved. Such as editor entities." );

		ImGui::Spacing( );
		ImGui::Separator( );
		ImGui::Spacing( );

		for ( auto component : e->GetComponents( ) )
		{
			if ( ImGui::CollapsingHeader( Pine::SComponentNames[ static_cast< int >( component->GetType( ) ) ], ImGuiTreeNodeFlags_DefaultOpen ) )
			{
				// Show active check box
				bool active = component->GetActive( );
				ImGui::Checkbox( std::string( "Active##" + std::to_string( e->GetId( ) ) + std::to_string( static_cast< int >( component->GetType( ) ) ) ).c_str( ), &active );
				component->SetActive( active );

				// Show the remove button
				if ( component != e->GetTransform( ) )
				{
					ImGui::SameLine( );
					
					if ( ImGui::Button( "Remove" ) )
					{
						e->RemoveComponent( component );
						break;
					}
				}

				// Render component properties			
				Gui::ComponentRenderer::Render( component );

			}
 		}

		ImGui::Separator( );
		
		if ( ImGui::Button( "Add new component...", ImVec2( -1.f, 30.f ) ) )
		{

		}
	}

	void DisplayAsset( Pine::IAsset* asset )
	{
		static auto unknownFileIcon = Pine::Assets::GetAsset<Pine::Texture2D>( "Assets\\Engine\\Icons\\030-corrupt file.png" );

		unsigned id = unknownFileIcon->GetId(  );

		bool invertUvs = false;
		
		if ( Editor::AssetPreviewGenerator::GenerateFullscreenAssetPreview( asset ) )
		{
			id = Editor::AssetPreviewGenerator::GetFullscreenFrameBuffer( )->GetTextureId( );
			invertUvs = true;
		}
		else if ( const auto preview = Editor::AssetPreviewGenerator::GetAssetPreview( asset ) )
		{
			id = preview->texture;
			invertUvs = preview->invertUVs;
		}
	
		ImGui::Image( reinterpret_cast< ImTextureID >( id ), ImVec2( 128.f, 128.f ), invertUvs ? ImVec2(1.f, 1.f) : ImVec2(0.f, 0.f), invertUvs ? ImVec2( 0.f, 0.f ) : ImVec2( 1.f, 1.f ) );
		ImGui::SameLine( );

		ImGui::BeginChild( "##AssetPreviewData", ImVec2( -1.f, 128.f ) );

		ImGui::Text( "%s", asset->GetFileName( ).c_str( ) );
		ImGui::Text( "%s", asset->GetPath( ).string( ).c_str( ) );
		ImGui::Text( "%s", Pine::SAssetType[ static_cast< int >( asset->GetType(  ) ) ] );
		
		ImGui::EndChild( );

		ImGui::Spacing( );
		ImGui::Separator( );
		ImGui::Spacing( );

		Gui::AssetRenderer::Render( asset );
	}

}

void Editor::Gui::Properties::Run( ) {

	const auto selectedEntity = Editor::Gui::GetSelectedEntity( );
	const auto selectedAsset = Editor::Gui::GetSelectedAsset( );

	ImGui::Begin( "Properties", nullptr, 0 );
	{
		if ( selectedEntity != nullptr ) {
			DisplayEntity( selectedEntity );
		}
		else if ( selectedAsset != nullptr ) {
			DisplayAsset( selectedAsset );
		}
		else {

			ImGui::Text( "Please select an entity or an asset to preview\nit's properties here." );
		}

	}
	ImGui::End( );

}
