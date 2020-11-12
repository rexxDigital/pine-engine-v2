#include "AssetIconGen.hpp"
#include <unordered_map>
#include <Pine\Assets\Assets.hpp>
#include "Pine\OpenGL\FrameBuffer\FrameBuffer.hpp"

namespace {

	struct AssetIcon_t {
		std::string m_Path = "";

		Pine::IAsset* m_Asset = nullptr;
		Pine::FrameBuffer* m_FrameBuffer = nullptr;
	
		Pine::Texture2D* m_Texture2D = nullptr;
	};

	std::unordered_map<std::string, std::unique_ptr<AssetIcon_t>> m_Icons;

}

Pine::Texture2D* Editor::Gui::Utility::AssetIconGen::GetAssetIcon( const std::string& str ) {
	if ( m_Icons.count( str ) == 0 ) {
		return nullptr;
	}

	return m_Icons[ str ]->m_Texture2D;
}

void Editor::Gui::Utility::AssetIconGen::Update( ) {
	// Remove unloaded icons
	for ( auto& iconElement : m_Icons ) {
		auto& icon = iconElement.second;
		
		// If the asset exists, don't dispose the icon.
		if ( Pine::Assets::GetAsset( icon->m_Path ) != nullptr ) 
			continue;

		// Dispose frame buffer
		if ( icon->m_FrameBuffer ) {
			icon->m_FrameBuffer->Dispose( );
			delete icon->m_FrameBuffer;
		}

		m_Icons.erase( iconElement.first );
	}

	// Loop through all assets, and add an icon for each of them.
	for ( auto& assetElement : Pine::Assets::GetAssets( ) ) {
		auto& asset = assetElement.second;
		
		AssetIcon_t* icon = nullptr;

		// Check if icon exists
		if ( m_Icons.count( asset->GetPath( ).string( ) ) != 0 ) {
			icon = m_Icons[ asset->GetPath( ).string( ) ].get( );
		}

		// Not found, add the new icon for the asset
		if ( icon == nullptr ) {
			auto newIcon = std::make_unique<AssetIcon_t>( );

			newIcon->m_Asset = asset;
			newIcon->m_Path = asset->GetPath( ).string( );

			m_Icons[ newIcon->m_Path ] = std::move( newIcon );

			icon = m_Icons[ asset->GetPath( ).string( ) ].get( );
		}

		// Update the asset icon, depending on it's type:
		switch ( asset->GetType( ) ) {
		case Pine::EAssetType::Texture2D:
			icon->m_Texture2D = dynamic_cast< Pine::Texture2D* >( asset );
			break;
		case Pine::EAssetType::Texture3D:
			icon->m_Texture2D = Pine::Assets::GetAsset<Pine::Texture2D>("Assets\\Editor\\Icons\\cube_map.png");
			break;
		case Pine::EAssetType::Material:
			icon->m_Texture2D = Pine::Assets::GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\image.png" );
			break;
		case Pine::EAssetType::Blueprint:
			icon->m_Texture2D = Pine::Assets::GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\blueprint.png" );
			break;
		case Pine::EAssetType::Level:
			icon->m_Texture2D = Pine::Assets::GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\level.png" );
			break;
		case Pine::EAssetType::Model:
			icon->m_Texture2D = Pine::Assets::GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\model.png" );
			break;
		case Pine::EAssetType::Shader:
			icon->m_Texture2D = Pine::Assets::GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\shader.png" );
			break;
		default: 
			icon->m_Texture2D = Pine::Assets::GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\corrupt.png" );
			break;
		}

	}
}
