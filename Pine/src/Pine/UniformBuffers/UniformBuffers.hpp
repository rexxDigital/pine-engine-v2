#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "../OpenGL/UniformBuffer/UniformBuffer.hpp"

namespace Pine::UniformBuffers {

	struct MatrixBufferData_t {
		glm::mat4 ProjectionMatrix;
		glm::mat4 ViewMatrix;
	};

	struct LightData_t {
		glm::vec3 position;
		float padding0;
		glm::vec3 color;
		float padding1;
	};

	struct LightBufferData_t {
		LightData_t lights[10];
	};

	struct MaterialBufferData_t {
		glm::vec3 diffuseColor;
		float padding0;
		glm::vec3 specularColor;
		float padding1;
		glm::vec3 ambientColor;
		float shininiess;
	};

	void Setup();
	void Dispose();

	MatrixBufferData_t* GetMatrixBufferData();
	LightBufferData_t* GetLightsBufferData();
	MaterialBufferData_t* GetMaterialBufferData();

	UniformBuffer* GetMatrixUniformBuffer();
	UniformBuffer* GetLightsUniformBuffer();
	UniformBuffer* GetMaterialUniformBuffer();
	
}