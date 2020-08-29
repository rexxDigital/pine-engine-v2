#pragma once
#include <string>
#include <glm/vec3.hpp>
#include "../../../Assets/Model/Model.hpp"

namespace Gui::Widgets
{

	void Vector3(const std::string& str, glm::vec3& vec);
	Pine::IAsset* AssetPicker(Pine::IAsset* currentAsset, const std::string& str, Pine::EAssetType type);

}
