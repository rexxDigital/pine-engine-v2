#include "AssetPropertiesRenderer.hpp"
#include <Pine\Assets\IAsset\IAsset.hpp>
#include <Pine\Assets\Texture2D\Texture2D.hpp>
#include <ImGui\imgui.h>

namespace {

	void RenderTexture2D( Pine::Texture2D* texture2d ) {
		constexpr float padding = 128.f;

		auto avaliableArea = ImGui::GetContentRegionAvail( );

		if ( 0.f > avaliableArea.x ) {
			return;
		}

		ImGui::Image( reinterpret_cast< ImTextureID >( texture2d->GetId( ) ), ImVec2( avaliableArea.x - padding, avaliableArea.x - padding ) );


//		ImGui::SameLine( );

		ImGui::BeginChild( "Texture2DProperties", ImVec2( -1.f, 90.f ), false );

		ImGui::Text( "OpenGL ID: %d", texture2d->GetId( ) );
		ImGui::Text( "Width: %d", texture2d->GetWidth( ) );
		ImGui::Text( "Height: %d", texture2d->GetWidth( ) );
		ImGui::Text( "Channels: %d", texture2d->GetChannels( ) );

		ImGui::EndChild( );
	}

}

void Editor::Gui::Utility::AssetPropertiesRenderer::RenderAssetProperties( Pine::IAsset* asset ) {
	if ( !asset )
		return;

	switch ( asset->GetType( ) ) {
	case Pine::EAssetType::Texture2D:
		RenderTexture2D( dynamic_cast< Pine::Texture2D* >( asset ) );
		break;
	default:
		break;
	}

}
