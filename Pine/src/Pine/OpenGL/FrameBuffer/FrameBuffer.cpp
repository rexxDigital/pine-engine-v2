#include "FrameBuffer.hpp"
#include <GL/glew.h>

Pine::FrameBuffer::FrameBuffer()
{
}

unsigned int Pine::FrameBuffer::GetId() const
{
	return m_Id;
}

unsigned int Pine::FrameBuffer::GetTextureId() const
{
	return m_TextureBuffer;
}

unsigned int Pine::FrameBuffer::GetDepthId() const
{
	return m_DepthBuffer;
}

int Pine::FrameBuffer::GetWidth() const {
	return m_Width;
}

int Pine::FrameBuffer::GetHeight() const {
	return m_Height;
}

void Pine::FrameBuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
}

void Pine::FrameBuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Pine::FrameBuffer::Dispose()
{
	glDeleteFramebuffers(1, &m_Id);

	glDeleteTextures(1, &m_TextureBuffer);
	glDeleteTextures(1, &m_DepthBuffer);
}

void Pine::FrameBuffer::Create(int width, int height)
{
	glGenFramebuffers(1, &m_Id);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Id);

	// Create texture buffer
	glGenTextures(1, &m_TextureBuffer);
	glBindTexture(GL_TEXTURE_2D, m_TextureBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Attach the texture to the frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureBuffer, 0);

	// Create a depth buffer
	glGenTextures(1, &m_DepthBuffer);
	glBindTexture(GL_TEXTURE_2D, m_DepthBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

	// Attach the depth buffer as well.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,  m_DepthBuffer, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_Width = width;
	m_Height = height;
}