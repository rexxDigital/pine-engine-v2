#pragma once

#define JSON_ASSERT(x)

#include <json.hpp>
#include <glm/vec3.hpp>

namespace Pine {
	class IAsset;
}

// Wrapper for JSON loading/writing.
namespace Pine::Serialization {

	nlohmann::json LoadJSONFromFile(const std::string& fileName);

	glm::vec3 LoadVec3(const nlohmann::json& j, const std::string& name);
	void SaveVec3( nlohmann::json& j, const glm::vec3& vec );

	void SaveAsset( nlohmann::json& j, IAsset* asset );
	IAsset* LoadAsset(const nlohmann::json& j, const std::string& name);

}