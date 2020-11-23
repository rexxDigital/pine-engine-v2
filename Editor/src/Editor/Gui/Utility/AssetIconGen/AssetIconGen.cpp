#include "AssetIconGen.hpp"
#include <unordered_map>
#include "Pine/Assets/Assets.hpp"
#include "Pine/OpenGL/FrameBuffer/FrameBuffer.hpp"

namespace {

	struct AssetIcon_t {
		std::string m_Path = "";
		Pine::IAsset* m_Asset = nullptr;

		Pine::FrameBuffer* m_FrameBuffer = nullptr;
		Pine::Texture2D* m_Texture = nullptr;
	};

	std::unordered_map<std::string, std::unique_ptr<AssetIcon_t>> m_Icons;

}

Pine::Texture2D* Editor::Gui::Utility::AssetIconGen::GetAssetIcon( const std::string& str )
{
	if ( m_Icons.count( str ) == 0 ) {
		return nullptr;
	}
}

void Editor::Gui::Utility::AssetIconGen::Update( )
{
	// Remove disposed icons
	for ( auto& icon : m_Icons ) {
		icon.second->m_Asset = Pine::Assets::GetAsset( icon.second->m_Path );
	
		// Remove asset if we couldn't find the asset.
		if ( !icon.second->m_Asset ) {
			// Dispose frame buffer
			if ( icon.second->m_FrameBuffer ) {
				icon.second->m_FrameBuffer->Dispose( );
				delete icon.second->m_FrameBuffer;
			}

			m_Icons.erase( icon.first );
		}
	}

	for ( auto& asset : Pine::Assets::GetAssets( ) ) {
		 
	}
}
