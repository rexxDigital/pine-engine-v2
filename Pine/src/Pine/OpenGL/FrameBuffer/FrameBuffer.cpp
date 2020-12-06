#include "FrameBuffer.hpp"
#include <GL/glew.h>

Pine::FrameBuffer::FrameBuffer( )
{
}

unsigned int Pine::FrameBuffer::GetId( ) const
{
	return m_Id;
}

unsigned int Pine::FrameBuffer::GetTextureId( ) const
{
	return m_TextureBuffer;
}

unsigned Pine::FrameBuffer::GetNormalBufferId( ) const
{
	return m_NormalBuffer;
}

unsigned int Pine::FrameBuffer::GetDepthId( ) const
{
	return m_DepthBuffer;
}

int Pine::FrameBuffer::GetWidth( ) const {
	return m_Width;
}

int Pine::FrameBuffer::GetHeight( ) const {
	return m_Height;
}

void Pine::FrameBuffer::Bind( ) const
{
	glBindFramebuffer( GL_FRAMEBUFFER, m_Id );
}

void Pine::FrameBuffer::Unbind( ) const
{
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void Pine::FrameBuffer::Dispose( )
{
	glDeleteFramebuffers( 1, &m_Id );

	glDeleteTextures( 1, &m_TextureBuffer );
	glDeleteTextures( 1, &m_DepthBuffer );
}

void Pine::FrameBuffer::Create( int width, int height, bool createNormal )
{
	glGenFramebuffers( 1, &m_Id );
	glBindFramebuffer( GL_FRAMEBUFFER, m_Id );

	// Create texture buffer
	glGenTextures( 1, &m_TextureBuffer );
	glBindTexture( GL_TEXTURE_2D, m_TextureBuffer );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// Attach the texture to the frame buffer
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureBuffer, 0 );

	// If we need to create a normal buffer
	if ( createNormal )
	{
		glGenTextures( 1, &m_NormalBuffer );
		glBindTexture( GL_TEXTURE_2D, m_NormalBuffer );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		// attach that bitch
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_NormalBuffer, 0 );

		unsigned int attachments[ 2 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

		glDrawBuffers( 2, attachments );
	}
	
	// Create a depth buffer
	glGenTextures( 1, &m_DepthBuffer );
	glBindTexture( GL_TEXTURE_2D, m_DepthBuffer );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	
	// Attach the depth buffer as well.
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthBuffer, 0 );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	m_Width = width;
	m_Height = height;
}