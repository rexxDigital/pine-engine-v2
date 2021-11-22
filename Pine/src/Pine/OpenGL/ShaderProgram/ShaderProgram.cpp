#include "ShaderProgram.hpp"
#include <GL/glew.h>
#include "../../Core/Log/Log.hpp"

uint32_t Pine::ShaderProgram::GetId( ) const
{
	return m_Id;
}

bool Pine::ShaderProgram::GetReady( ) const
{
	return m_IsReady;
}

bool Pine::ShaderProgram::CompileShader( const std::string& src, EShaderType typeEnum )
{
	// Translate the enum type to the OpenGL type, for instance GL_VERTEX_ARRAY.
	int32_t type = -1;

	switch ( typeEnum )
	{
	case EShaderType::Vertex:
		type = GL_VERTEX_SHADER;
		break;
	case EShaderType::Fragment:
		type = GL_FRAGMENT_SHADER;
		break;
	case EShaderType::Compute:
		type = GL_COMPUTE_SHADER;
		break;
	default:
		break;
	}

	if ( type == -1 )
	{
		Log->Error( "Failed to compile shader, invalid shader type." );
		return false;
	}

	const int32_t shader = glCreateShader( type );
	const char* c_str = src.c_str( );

	// Set the shader source and attempt to compile.
	glShaderSource( shader, 1, &c_str, nullptr );
	glCompileShader( shader );

	// Check the compile status, if it failed or not.
	int32_t compileStatus = 0;

	glGetShaderiv( shader, GL_COMPILE_STATUS, &compileStatus );

	if ( compileStatus == GL_FALSE )
	{
		// Get the error message length
		int32_t errorStrLength = 0;
		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &errorStrLength );

		// Get the error message itself
		std::vector<char> errorMessageArray( errorStrLength );
		glGetShaderInfoLog( shader, errorStrLength, &errorStrLength, &errorMessageArray[ 0 ] );

		// Get a char array pointer from the vector, since it has a
		// NULL character, this will work fine.
		const char* errorMessage = errorMessageArray.data( );

		Log->Error( "Failed to compile shader, error: " + std::string( errorMessage ) );

		return false;
	}

	// Since the shader is ready, add into the list for linking later.
	m_Shaders.push_back( shader );

	return true;
}

void Pine::ShaderProgram::LinkProgram( )
{
	// Do we have any shaders to link?
	if ( m_Shaders.empty( ) )
	{
		// Not going to log anything here, since there will probably be a shader error anyway.
		return;
	}

	m_Id = glCreateProgram( );

	// Attach each shader to the program
	for ( auto& shader : m_Shaders )
	{
		glAttachShader( m_Id, shader );
	}

	// Attempt to link the program
	glLinkProgram( m_Id );

	// Check for any linking errors, the same way we check for shader errors
	int32_t linkStatus = 0;

	glGetProgramiv( m_Id, GL_LINK_STATUS, &linkStatus );

	if ( linkStatus == GL_FALSE )
	{
		// Get the error message length
		int32_t errorStrLength = 0;
		glGetProgramiv( m_Id, GL_INFO_LOG_LENGTH, &errorStrLength );

		// Get the error message itself
		std::vector<char> errorMessageArray( errorStrLength );
		glGetProgramInfoLog( m_Id, errorStrLength, &errorStrLength, &errorMessageArray[ 0 ] );

		// Get a char array pointer from the vector, since it has a
		// NULL escape character, this will work fine.
		const char* errorMessage = errorMessageArray.data( );
	
		Log->Error( "Failed to link program: " + std::string( errorMessage ) );

		// Pine::ShaderProgram::DisposeShader( ) should dispose the remaining shaders
		// if the application closes or the shader is reloaded.

		return;
	}

	// Since the link was successful, we can now safely detach and remove them.
	for ( auto shader : m_Shaders )
	{
		glDetachShader( m_Id, shader );
		glDeleteShader( shader );
	}

	m_Shaders.clear( );
	m_IsReady = true;
}

Pine::UniformVariable* Pine::ShaderProgram::GetUniformVariable( const std::string& variableName )
{
	if ( m_UniformVariables.count( variableName ) == 0 ) 
	{
		m_UniformVariables[ variableName ] = new UniformVariable( this, variableName );
	}

	return m_UniformVariables[ variableName ];
}

void Pine::ShaderProgram::SetupUniformBuffer( const UniformBuffer* buffer, const std::string& name ) const
{
	const int bufferIndex = glGetUniformBlockIndex( m_Id, name.c_str( ) );

	if ( 0 > bufferIndex ) 
	{
		Log->Error( "Failed to find uniform buffer, " + name );
	}

	glUniformBlockBinding( m_Id, bufferIndex, buffer->GetBinding( ) );
}

void Pine::ShaderProgram::DisposeShader( )
{
	// We could have some shaders left, after an unsuccessful link, if they exist, remove them.
	for ( auto shader : m_Shaders )
	{
		glDeleteShader( shader );
	}

	for ( auto uniVariable : m_UniformVariables ) {
		delete uniVariable.second;
	}

	m_UniformVariables.clear( );

	glDeleteProgram( m_Id );
}

void Pine::ShaderProgram::Use( ) const
{
	glUseProgram( m_Id );
}
