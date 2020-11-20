#pragma once
#include <glm\glm.hpp>
#include <string>
#include <Pine\Assets\IAsset\IAsset.hpp>


namespace Pine { class Texture2D; }

namespace Editor::Gui::Widgets {

	bool Checkbox( const std::string& str, bool& value );
	void Vector3( const std::string& str, glm::vec3& vec );
	bool SliderFloat(const std::string& str, float& value, float min, float max, const std::string& format);
	
	bool ColorPicker( const std::string& str, glm::vec3& vec );

	Pine::IAsset* AssetPicker( const std::string& str, Pine::IAsset* currentAsset = nullptr, bool shouldRestrictType = false, Pine::EAssetType type = Pine::EAssetType::Invalid );

	bool Icon( const std::string& text, bool showBackground, Pine::Texture2D* texture, int size = 48, Pine::IAsset* asset = nullptr );

	void PushDisabled( );
	void PopDisabled( );

}