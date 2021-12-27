#pragma once
#include "../Assets.hpp"
#include "../../OpenGL/ShaderProgram/ShaderProgram.hpp"
#include <json.hpp>

namespace Pine
{

	enum ShaderProperties
	{
		TransparencyDiscard = ( 1 << 0 ),
		TransparencyBlend = ( 1 << 1 ),
		PerformanceFast = ( 1 << 2 )
	};

	class Shader : public IAsset
	{
	private:
		std::vector<IAsset*> m_AttachedShaderFiles;
		std::vector<ShaderProgram*> m_ShaderPrograms;

		ShaderProgram* m_SelectedShaderProgram = nullptr;

		std::unordered_map<std::uint32_t, int> m_ShaderPropertiesMap;

		ShaderProgram* CompileShaderProgram( nlohmann::json& j, const std::vector<std::string>& versionMacros );
	public:
		Shader( );

		const std::vector<IAsset*> GetAttachedShaderFiles( ) const;

		void Use( int version = 0 );
		UniformVariable* GetUniformVariable( const std::string& variableName );

		bool LoadFromFile( ) override;
		bool SaveToFile( ) override;
		void Dispose( ) override;
	};

}