#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "../../OpenGL/UniformBuffer/UniformBuffer.hpp"

namespace Pine::UniformBuffers
{

	struct MatrixBufferData_t
	{
		glm::mat4 ProjectionMatrix;
		glm::mat4 ViewMatrix;
	};

	struct LightData_t
	{
		glm::vec3 position;
		float padding0;
		glm::vec3 rotation;
		float padding1;
		glm::vec3 color;
		float padding2;
		glm::vec3 attenuation;
		float cutOffAngle;
		float cutOffSmoothness;
	};

	struct LightBufferData_t
	{
		LightData_t lights[ 4 ];
	};

	struct MaterialBufferData_t
	{
		glm::vec3 diffuseColor;
		float padding0;
		glm::vec3 specularColor;
		float padding1;
		glm::vec3 ambientColor;
		float shininiess;
		float textureScale;
	};

	struct TransformData_t
	{
		glm::mat4 transform[ 32 ];
	};

	void Setup( );
	void Dispose( );

	MatrixBufferData_t* GetMatrixBufferData( );
	LightBufferData_t* GetLightsBufferData( );
	MaterialBufferData_t* GetMaterialBufferData( );
	TransformData_t* GetTransformBufferData( );

	UniformBuffer* GetMatrixUniformBuffer( );
	UniformBuffer* GetLightsUniformBuffer( );
	UniformBuffer* GetMaterialUniformBuffer( );
	UniformBuffer* GetTransformDataUniformBuffer( );

}