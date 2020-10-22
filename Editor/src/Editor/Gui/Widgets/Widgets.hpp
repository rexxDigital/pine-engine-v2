#pragma once
#include <glm\glm.hpp>
#include <string>
#include <Pine\Assets\IAsset\IAsset.hpp>

namespace Editor::Gui::Widgets {

	bool Checkbox( const std::string& str, bool& value );
	void Vector3( const std::string& str, glm::vec3& vec );

	Pine::IAsset* AssetPicker( const std::string& str, Pine::IAsset* currentAsset = nullptr, bool shouldRestrictType = false, Pine::EAssetType type = Pine::EAssetType::Invalid );

	void PushDisabled( );
	void PopDisabled( );

}