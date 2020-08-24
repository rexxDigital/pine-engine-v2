#include "Shader.hpp"
#include <json.hpp>
#include <fstream>
#include "../../Core/Serialization/Serialization.hpp"
#include "../../Core/Log/Log.hpp"
#include "../../UniformBuffers/UniformBuffers.hpp"

namespace {

	bool LoadShader(const nlohmann::json& j, Pine::Shader* shader, const std::string& name, const Pine::EShaderType type) {
		try {
			if (j.find(name) == j.end()) {
				return false;
			}

			const std::string shaderDirectory = shader->GetPath().parent_path().string();
			const std::string filePath = shaderDirectory + "\\" + j[name].get<std::string>();
			
			if (!std::filesystem::exists(filePath)) {
				Pine::Log::Warning("Unable to find shader at path " + filePath);
				return false;
			}

			std::ifstream stream(filePath);

			std::string str((std::istreambuf_iterator<char>(stream)),
				std::istreambuf_iterator<char>());

			stream.close();

			return shader->CompileShader(str, type);
		}
		catch (std::exception e) {
			return false;
		}

		return false;
	}

}

Pine::Shader::Shader() {
	m_Type = EAssetType::Shader;
}

bool Pine::Shader::LoadFromFile() {
    auto j = Serialization::LoadJSONFromFile(m_FilePath.string());
   
	LoadShader(j, this, "VertexShader", EShaderType::Vertex);
	LoadShader(j, this, "FragmentShader", EShaderType::Fragment);
	LoadShader(j, this, "ComputeShader", EShaderType::Compute);

	LinkProgram();

	// Setup samplers
	if (j.contains("Sampler2D")) {
		for (auto& item : j["Sampler2D"].items()) {
			if (item.key() == "Diffuse") {
				this->GetUniformVariable(item.value())->LoadInteger(0);
			}
			else if (item.key() == "Specular") {
				this->GetUniformVariable(item.value())->LoadInteger(1);
			}
		}
	}

	// Setup uniform buffers
	if (j.contains("UniformBuffers")) {
		for (auto& item : j["UniformBuffers"].items()) {
			if (item.key() == "Matrices") {
				this->SetupUniformBuffer(UniformBuffers::GetMatrixUniformBuffer(), item.value());
			}

			if (item.key() == "Lights") {
				this->SetupUniformBuffer(UniformBuffers::GetLightsUniformBuffer(), item.value());
			}
		
			if (item.key() == "Material") {
				this->SetupUniformBuffer(UniformBuffers::GetMaterialUniformBuffer(), item.value());
			}
		}
	}


    return true;
}

bool Pine::Shader::SaveToFile() {
    return false;
}

void Pine::Shader::Dispose() {
    DisposeShader();
}
