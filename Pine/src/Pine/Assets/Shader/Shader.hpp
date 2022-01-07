#pragma once
#include "../Assets.hpp"
#include "../../OpenGL/ShaderProgram/ShaderProgram.hpp"
#include <json.hpp>

namespace Pine
{

	enum ShaderProperties
	{
		TransparencyDiscard = ( 1 << 0 ),
		PerformanceFast = ( 1 << 1 )
	};

	class Shader : public IAsset
	{
	private:
		std::vector<IAsset*> m_AttachedShaderFiles;
		std::vector<ShaderProgram*> m_ShaderPrograms;

		ShaderProgram* m_SelectedShaderProgram = nullptr;

		std::unordered_map<std::uint32_t, int> m_ShaderPropertiesMap;

		bool m_SupportsInstancedRendering = false;

		ShaderProgram* CompileShaderProgram( nlohmann::json& j, const std::vector<std::string>& versionMacros );
	public:
		Shader( );

		const std::vector<IAsset*> GetAttachedShaderFiles( ) const;

		void Use( int version = 0 );
		UniformVariable* GetUniformVariable( const std::string& variableName );

		bool GetSupportsInstancedRendering( ) const;

		bool LoadFromFile( ) override;
		bool SaveToFile( ) override;
		void Dispose( ) override;
	};

}