#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "../UniformVariable/UniformVariable.hpp"
#include "../UniformBuffer/UniformBuffer.hpp"

namespace Pine
{
	enum class EShaderType
	{
		Invalid,
		Vertex,
		Fragment,
		Compute
	};

	class ShaderProgram
	{
	private:
		uint32_t m_Id;
		std::vector<uint32_t> m_Shaders;
		bool m_IsReady = false;

		std::unordered_map<std::string, UniformVariable*> m_UniformVariables;
	public:

		uint32_t GetId() const;
		bool GetReady() const;

		bool CompileShader(const std::string& src, EShaderType type);
		void LinkProgram();

		UniformVariable* GetUniformVariable(const std::string& variableName);
		void SetupUniformBuffer(const UniformBuffer* buffer, const std::string& name);

		void DisposeShader();

		void Use() const;

	};

}