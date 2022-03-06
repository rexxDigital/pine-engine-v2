#pragma once
#include <string>
#include "Pine\Assets\Texture2D\Texture2D.hpp"

namespace Editor::Gui::Utility::AssetIcon {

	struct AssetIcon_t
	{
		std::string m_Path;

		Pine::IAsset* m_Asset = nullptr;

		Pine::FrameBuffer* m_FrameBuffer = nullptr;
		Pine::Texture2D* m_FrameBufferTexture = nullptr;

		Pine::Texture2D* m_Texture2D = nullptr;

		bool m_LiveIconDirty = true;
		bool m_LiveIconReady = false;
	};

	AssetIcon_t* GetAssetIcon( const std::string& str );

	Pine::Texture2D* GetThumbnailTexture( );

	void Update( );

	void Render( );

}