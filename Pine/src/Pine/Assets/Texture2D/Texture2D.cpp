#include "Texture2D.hpp"
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../../Core/Log/Log.hpp"
#include "../../OpenGL/FrameBuffer/FrameBuffer.hpp"

Pine::Texture2D::Texture2D( ) {
	m_Type = AssetType::Texture2D;
}

unsigned int Pine::Texture2D::GetId( ) const {
	return m_Id;
}

int Pine::Texture2D::GetWidth( ) const {
	return m_Width;
}

int Pine::Texture2D::GetHeight( ) const {
	return m_Height;
}

int Pine::Texture2D::GetChannels( ) const {
	return m_Channels;
}

void Pine::Texture2D::CreateFromFrameBuffer( Pine::FrameBuffer* frameBuffer )
{
	m_Width = frameBuffer->GetWidth( );
	m_Height = frameBuffer->GetHeight( );
	m_Id = frameBuffer->GetTextureId( );
	m_Channels = 4;
}

void Pine::Texture2D::CreateFromData( int width, int height, int format, void* data )
{
	glGenTextures( 1, &m_Id );
	glBindTexture( GL_TEXTURE_2D, m_Id );
	
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, format, GL_FLOAT, data );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	m_Width = width;
	m_Height = height;
	m_Channels = format == GL_RGB ? 3 : 4;
}

bool Pine::Texture2D::LoadFromFile( ) {
	int width, height, channels;

	const auto data = stbi_load( m_FilePath.string( ).c_str( ), &width, &height, &channels, 0 );

	if ( !data ) {
		stbi_image_free( data );

		Log->Error( "Failed to load texture, " + m_FilePath.string( ) );

		return false;
	}

	glGenTextures( 1, &m_Id );
	glBindTexture( GL_TEXTURE_2D, m_Id );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// clean readable code :tm:
	const int format = channels == 1 ? GL_R8 : channels == 4 ? GL_RGBA : GL_RGB;

	glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data );

	glGenerateMipmap( GL_TEXTURE_2D );

	m_Width = width;
	m_Height = height;
	m_Channels = channels;

	stbi_image_free( data );

	return true;
}

bool Pine::Texture2D::SaveToFile( ) {
	return false;
}

void Pine::Texture2D::Dispose( ) {
	glDeleteTextures( 1, &m_Id );
}
