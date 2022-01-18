#include "FrameBuffer.hpp"
#include "../../Core/Log/Log.hpp"
#include <GL/glew.h>

Pine::FrameBuffer::FrameBuffer( ) = default;

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
	return m_DepthStencilBuffer;
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

void Pine::FrameBuffer::Dispose( ) const
{
	glDeleteFramebuffers( 1, &m_Id );

	glDeleteTextures( 1, &m_TextureBuffer );
	glDeleteTextures( 1, &m_DepthStencilBuffer );
}

void Pine::FrameBuffer::Create( int width, int height, bool createNormal, bool multiSample )
{
	glGenFramebuffers( 1, &m_Id );
	glBindFramebuffer( GL_FRAMEBUFFER, m_Id );

	auto textureType = multiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

	// Create texture buffer
	glGenTextures( 1, &m_TextureBuffer );
	glBindTexture( textureType, m_TextureBuffer );

	if ( multiSample )
		glTexImage2DMultisample( textureType, 16, GL_RGBA, width, height, GL_TRUE );
	else
		glTexImage2D( textureType, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr );

	glTexParameteri( textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// Attach the texture to the frame buffer
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureType, m_TextureBuffer, 0 );

	// If we need to create a normal buffer
	if ( createNormal )
	{
		glGenTextures( 1, &m_NormalBuffer );
		glBindTexture( GL_TEXTURE_2D, m_NormalBuffer );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_NormalBuffer, 0 );

		unsigned int attachments[ 2 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

		glDrawBuffers( 2, attachments );
	}

	// Create a depth & stencil buffer
	glGenTextures( 1, &m_DepthStencilBuffer );
	glBindTexture( textureType, m_DepthStencilBuffer );

	if ( multiSample )
		glTexImage2DMultisample( textureType, 16, GL_DEPTH24_STENCIL8, width, height, GL_TRUE );
	else
		glTexImage2D( textureType, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr );

	glTexParameteri( textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, textureType, m_DepthStencilBuffer, 0 );

	const auto status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		Log->Error( "Failure in creating frame buffer: " + std::to_string( status ) );
	}

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	m_Width = width;
	m_Height = height;
}

void Pine::FrameBuffer::BlitMultisample( FrameBuffer* target )
{
	const auto width = target->GetWidth( );
	const auto height = target->GetHeight( );

	glBindFramebuffer( GL_READ_FRAMEBUFFER, m_Id );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, target->GetId(  ) );

	glBlitFramebuffer( 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST );

	glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
}
