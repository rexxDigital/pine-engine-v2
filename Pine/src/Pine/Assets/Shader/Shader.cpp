#include "Shader.hpp"
#include <json.hpp>
#include <fstream>
#include "../../Core/Serialization/Serialization.hpp"
#include "../../Core/Log/Log.hpp"
#include "../../Rendering/UniformBuffers/UniformBuffers.hpp"
#include "../../Core/String/String.hpp"

namespace
{

	bool LoadShader( const nlohmann::json& j, Pine::Shader* shader, Pine::ShaderProgram* program, const std::string& name, const Pine::EShaderType type, std::vector<Pine::IAsset*>& shaderFiles, const std::vector<std::string>& versionMacros )
	{
		try
		{
			if ( j.find( name ) == j.end( ) )
			{
				return false;
			}

			const std::string shaderDirectory = shader->GetPath( ).parent_path( ).string( );
			const std::string filePath = shaderDirectory + "\\" + j[ name ].get<std::string>( );

			if ( !std::filesystem::exists( filePath ) )
			{
				Pine::Log->Warning( "Unable to find shader at path " + filePath );
				return false;
			}

			shaderFiles.push_back( Pine::Assets->LoadFromFile( filePath ) );

			std::ifstream stream( filePath );

			std::string str( ( std::istreambuf_iterator<char>( stream ) ),
							 std::istreambuf_iterator<char>( ) );

			stream.close( );

			// Include the macros for the version
			std::string src = "";

			src += "#version 420 core\n";

			for ( const auto& macro : versionMacros )
			{
				src += "#define " + macro + "\n";
			}

			src += str;

			return program->CompileShader( src, type );
		}
		catch ( std::exception& e )
		{
			return false;
		}
	}

}

Pine::ShaderProgram* Pine::Shader::CompileShaderProgram( nlohmann::json& j, const std::vector<std::string>& versionMacros )
{
	const auto shaderProgram = new ShaderProgram( );

	LoadShader( j, this, shaderProgram, "VertexShader", EShaderType::Vertex, m_AttachedShaderFiles, versionMacros );
	LoadShader( j, this, shaderProgram, "FragmentShader", EShaderType::Fragment, m_AttachedShaderFiles, versionMacros );
	LoadShader( j, this, shaderProgram, "ComputeShader", EShaderType::Compute, m_AttachedShaderFiles, versionMacros );

	shaderProgram->LinkProgram( );

	// Setup samplers
	if ( j.contains( "Samplers" ) )
	{
		for ( auto& item : j[ "Samplers" ].items( ) )
		{
			if ( item.key( ) == "Diffuse" )
			{
				shaderProgram->GetUniformVariable( item.value( ) )->LoadInteger( 0 );
			}
			else if ( item.key( ) == "Specular" )
			{
				shaderProgram->GetUniformVariable( item.value( ) )->LoadInteger( 1 );
			}
			else if ( item.key( ) == "EnvMap" )
			{
				shaderProgram->GetUniformVariable( item.value( ) )->LoadInteger( 2 );
			}
		}
	}

	// Setup uniform buffers
	if ( j.contains( "UniformBuffers" ) )
	{
		for ( auto& item : j[ "UniformBuffers" ].items( ) )
		{
			if ( item.key( ) == "Matrices" )
			{
				shaderProgram->SetupUniformBuffer( UniformBuffers::GetMatrixUniformBuffer( ), item.value( ) );
			}

			if ( item.key( ) == "Lights" )
			{
				shaderProgram->SetupUniformBuffer( UniformBuffers::GetLightsUniformBuffer( ), item.value( ) );
			}

			if ( item.key( ) == "Material" )
			{
				shaderProgram->SetupUniformBuffer( UniformBuffers::GetMaterialUniformBuffer( ), item.value( ) );
			}

			if ( item.key( ) == "TransformData" )
			{
				shaderProgram->SetupUniformBuffer( UniformBuffers::GetTransformDataUniformBuffer( ), item.value( ) );
			}
		}
	}

	return shaderProgram;
}

Pine::Shader::Shader( )
{
	m_Type = AssetType::Shader;
}

const std::vector<Pine::IAsset*> Pine::Shader::GetAttachedShaderFiles( ) const
{
	return m_AttachedShaderFiles;
}

void Pine::Shader::Use( const int version )
{
	int index;

	if ( version == 0 )
	{
		index = 0;
	}
	else
	{
		index = m_ShaderPropertiesMap[ version ];
	}

	m_SelectedShaderProgram = m_ShaderPrograms[ index ];
	m_ShaderPrograms[ index ]->Use( );
}

Pine::UniformVariable* Pine::Shader::GetUniformVariable( const std::string& variableName )
{
	if ( !m_SelectedShaderProgram ) return nullptr;

	return m_SelectedShaderProgram->GetUniformVariable( variableName );
}

bool Pine::Shader::GetSupportsInstancedRendering( ) const
{
	return m_SupportsInstancedRendering;
}

bool Pine::Shader::LoadFromFile( )
{
	auto j = Serialization::LoadJSONFromFile( m_FilePath.string( ) );

	// First compile the normal version
	m_ShaderPrograms.push_back( CompileShaderProgram( j, { } ) );

	// Compile the other versions
	for ( const auto& version : j[ "Versions" ].items( ) )
	{
		m_ShaderPrograms.push_back( CompileShaderProgram( j, String::Split( version.key( ), "&&" ) ) );

		m_ShaderPropertiesMap[ version.value( ).get<uint32_t>( ) ] = m_ShaderPrograms.size( ) - 1;
	}

	for ( const auto shaderFileAsset : m_AttachedShaderFiles )
	{
		shaderFileAsset->SetMapped( true );
	}

	if ( j.contains( "SupportsInstancedRendering" ) )
		m_SupportsInstancedRendering = j[ "SupportsInstancedRendering" ].get<bool>( );

	return true;
}

bool Pine::Shader::SaveToFile( )
{
	return false;
}

void Pine::Shader::Dispose( )
{
	for ( const auto program : m_ShaderPrograms )
	{
		program->DisposeShader( );
		delete program;
	}

	m_ShaderPrograms.clear( );
	m_AttachedShaderFiles.clear( );
}
