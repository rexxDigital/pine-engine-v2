#include "UniformBuffer.hpp"
#include <GL/glew.h>

void Pine::UniformBuffer::Create( int size, int binding )
{
	glGenBuffers( 1, &m_Id );

	glBindBuffer( GL_UNIFORM_BUFFER, m_Id );
	glBufferData( GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW );
	glBindBufferBase( GL_UNIFORM_BUFFER, binding, m_Id );

	m_Binding = binding;
}

void Pine::UniformBuffer::Bind( ) const
{
	glBindBuffer( GL_UNIFORM_BUFFER, m_Id );
}

void Pine::UniformBuffer::Dispose( )
{
	glDeleteBuffers( 1, &m_Id );
}

uint32_t Pine::UniformBuffer::GetId( ) const
{
	return m_Id;
}

int Pine::UniformBuffer::GetBinding( ) const
{
	return m_Binding;
}

void Pine::UniformBuffer::UploadData( int offset, int size, void* data )
{
	glBufferSubData( GL_UNIFORM_BUFFER, offset, size, data );
}
