#pragma once
#include <string>
#include "Pine/Assets/Texture2D/Texture2D.hpp"

namespace Editor::Gui::Utility::AssetIconGen {

	Pine::Texture2D* GetAssetIcon( const std::string& str );

	void Update( );

}