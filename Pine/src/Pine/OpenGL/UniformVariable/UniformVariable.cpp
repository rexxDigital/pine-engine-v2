#include "UniformVariable.hpp"
#include <GL/glew.h>
#include "../../Core/Log/Log.hpp"
#include "../ShaderProgram/ShaderProgram.hpp"

Pine::UniformVariable::UniformVariable( const ShaderProgram* program, const std::string& name )
{
	m_Location = glGetUniformLocation( program->GetId( ), name.c_str( ) );
	m_Valid = m_Location >= 0;

	if ( !m_Valid ) 
	{
		//Log->Warning( "Failed to find uniform variable, " + name );
	}
}

int Pine::UniformVariable::GetLocation( ) const
{
	return m_Location;
}

bool Pine::UniformVariable::IsValid( ) const
{
	return m_Valid;
}

void Pine::UniformVariable::LoadInteger( int value )
{
	glUniform1i( m_Location, value );
}

void Pine::UniformVariable::LoadFloat( float value )
{
	glUniform1f( m_Location, value );
}

void Pine::UniformVariable::LoadVector2( const glm::vec2& value )
{
	glUniform2f( m_Location, value.x, value.y );
}

void Pine::UniformVariable::LoadVector3( const glm::vec3& value )
{
	glUniform3f( m_Location, value.x, value.y, value.z );
}

void Pine::UniformVariable::LoadVector4( const glm::vec4& value )
{
	glUniform4f( m_Location, value.x, value.y, value.z, value.w );
}

void Pine::UniformVariable::LoadMatrix4( const glm::mat4& value )
{
	glUniformMatrix4fv( m_Location, 1, false, &value[ 0 ][ 0 ] );
}

void Pine::UniformVariable::LoadVector2( const glm::ivec2& value )
{
    glUniform2i( m_Location, value.x, value.y );
}

void Pine::UniformVariable::LoadVector3( const glm::ivec3& value )
{
    glUniform3i( m_Location, value.x, value.y, value.z );
}

void Pine::UniformVariable::LoadVector4(const glm::ivec4 &value)
{
    glUniform4i(m_Location, value.x, value.y, value.z, value.w);
}
