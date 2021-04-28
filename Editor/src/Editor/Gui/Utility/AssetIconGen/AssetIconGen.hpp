#pragma once
#include <string>
#include "Pine\Assets\Texture2D\Texture2D.hpp"

namespace Editor::Gui::Utility::AssetIconGen {

	// Meant to be displayed as 64x64 image in the asset browser.
	Pine::Texture2D* GetAssetIcon( const std::string& str );

	// Meant to be a big ass 256x256 image as a thumbnail.
	// Within materials for example, it will render a sphere with the asset.
	Pine::Texture2D* GenerateAssetThumbnail( const std::string& str );

	void Update( );

}