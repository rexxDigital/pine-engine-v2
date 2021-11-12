#include "Texture3D.hpp"
#include <GL/glew.h>
#include "../../Core/Serialization/Serialization.hpp"
#include "../../Core/Log/Log.hpp"

#include <stb_image.h>

Pine::Texture3D::Texture3D( ) {
	m_Type = EAssetType::Texture3D;
}

unsigned int Pine::Texture3D::GetId( ) const {
	return m_Id;
}

void Pine::Texture3D::Bind( ) const {
	glBindTexture( GL_TEXTURE_CUBE_MAP, m_Id );
}

bool Pine::Texture3D::LoadFromFile( ) {
	const auto j = Pine::Serialization::LoadJSONFromFile( m_FilePath.string( ) );

	// file names in order.
	std::vector<std::string> fileNames;

	try {

		fileNames.push_back( j[ "Right" ] );
		fileNames.push_back( j[ "Left" ] );
		fileNames.push_back( j[ "Top" ] );
		fileNames.push_back( j[ "Bottom" ] );
		fileNames.push_back( j[ "Front" ] );
		fileNames.push_back( j[ "Back" ] );

	}
	catch ( ... ) {
		return false;
	}

	if ( fileNames.size( ) != 6 ) {
		Log->Error( "Failed to load cube map due to not having 6 textures." );
		return false;
	}

	glGenTextures( 1, &m_Id );
	glBindTexture( GL_TEXTURE_CUBE_MAP, m_Id );

	int width, height, nrChannels;
	unsigned char* data;

	const std::string baseDir = m_FilePath.parent_path( ).string( ) + "\\";

	for ( int i = 0; i < fileNames.size( ); i++ ) {
		const std::string fileName = baseDir + fileNames[ i ];

		data = stbi_load( fileName.c_str( ), &width, &height, &nrChannels, 0 );

		if ( !data ) {
			Log->Error( "Failed to load texture, " + fileName );

			glDeleteTextures( 1, &m_Id );
			stbi_image_free( data );
		}

		const int format = nrChannels == 1 ? GL_R8 : nrChannels == 4 ? GL_RGBA : GL_RGB;

		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data );
		stbi_image_free( data );
	}

	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

	return true;
}

bool Pine::Texture3D::SaveToFile( ) {
	return false;
}

void Pine::Texture3D::Dispose( ) {
	glDeleteTextures( 1, &m_Id );
}
