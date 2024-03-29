#pragma once
#include <glm/glm.hpp>
#include <string>
#include <Pine/Assets/IAsset/IAsset.hpp>

namespace Pine { class Texture2D; class Entity; }

// This fucking sucks...
struct PickerReturn
{
	bool valid = false;
	Pine::IAsset* asset = nullptr;
	Pine::Entity* entity = nullptr;
};

struct PathItem_t;

namespace Editor::Gui::Widgets {

	bool Checkbox( const std::string& str, bool& value );
	bool Vector3( const std::string& str, glm::vec3& vec );
	bool SliderFloat( const std::string& str, float& value, float min, float max, const std::string& format );

    bool Vector3Toggle( const std::string& str, std::array<bool, 3>& vec );

    bool Combobox( const std::string& str, int& value, const char* items );

	bool ColorPicker( const std::string& str, glm::vec3& vec );

	PickerReturn AssetPicker( const std::string& str, Pine::IAsset* currentAsset = nullptr, bool shouldRestrictType = false, Pine::AssetType type = Pine::AssetType::Invalid );
	PickerReturn EntityPicker( const std::string& str, Pine::Entity* currentEntity = nullptr );

	bool Icon( const std::string& text, bool showBackground, Pine::Texture2D* texture, int size = 48, Pine::IAsset* asset = nullptr, PathItem_t* directory = nullptr, bool flipImage = false );

	void PushDisabled( );
	void PopDisabled( );

}