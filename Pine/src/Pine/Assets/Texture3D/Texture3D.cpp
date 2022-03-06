#include "Texture3D.hpp"
#include <GL/glew.h>
#include "../../Core/Serialization/Serialization.hpp"
#include "../../Core/Log/Log.hpp"

#include <stb_image.h>
#include <fstream>

#include "../Assets.hpp"
#include "../Texture2D/Texture2D.hpp"

void Pine::Texture3D::Rebuild( )
{
	for ( int i = 0; i < 6;i++ )
	{
		if ( m_MultiTextures[ i ] == nullptr )
		{
			Log->Warning( "Failed to rebuild cube-map, missing textures." );
			return;
		}
	}

	glGenTextures( 1, &m_Id );
	glBindTexture( GL_TEXTURE_CUBE_MAP, m_Id );

	int width, height, nrChannels;
	unsigned char* data;

	for ( int i = 0; i < 6; i++ )
	{
		data = stbi_load( m_MultiTextures[ i ]->GetPath(  ).string(  ).c_str(  ), &width, &height, &nrChannels, 0 );

		if ( !data )
		{
			Log->Error( "Failed to load texture, " + m_MultiTextures[ i ]->GetPath(  ).string( ) );

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
}

Pine::Texture3D::Texture3D( )
{
	m_Type = AssetType::Texture3D;
}

unsigned int Pine::Texture3D::GetId( ) const
{
	return m_Id;
}

void Pine::Texture3D::SetTexture3DType( Texture3DType type )
{
	m_Texture3DType = type;
}

Pine::Texture3DType Pine::Texture3D::GetTexture3DType( ) const
{
	return m_Texture3DType;
}

void Pine::Texture3D::SetMultiTexture( Texture3DSide side, Texture2D* texture )
{
	assert( side >= 0 );

	m_MultiTextures[ side ] = texture;
	m_Updated = true;

	Rebuild( );
}

Pine::Texture2D* Pine::Texture3D::GetMultiTexture( Texture3DSide side ) const
{
	assert( side >= 0 );

	return m_MultiTextures[ side ];
}

void Pine::Texture3D::Bind( ) const
{
	glBindTexture( GL_TEXTURE_CUBE_MAP, m_Id );
}

bool Pine::Texture3D::LoadFromFile( )
{
	const auto j = Pine::Serialization::LoadJSONFromFile( m_FilePath.string( ) );

	// this is kinda stupid
	static const char* sideLookupTable[ ] = { "Right", "Left", "Top", "Bottom", "Front", "Back" };

	for ( int i = 0; i < 6;i++ )
	{
		if ( !j.contains( sideLookupTable[ i ] ) )
		{
			Log->Warning( "Failed to load texture 3d, missing textures." );
			break;
		}

		const auto texture = new Pine::Texture2D( );
		const auto path = j[ sideLookupTable[ i ] ].get<std::string>(  );

		texture->SetFilePath( path );

		if ( Assets->GetAsset( path ) != nullptr )
		{
			Assets->DisposeAsset( path );
			Assets->MapAsset( texture, path );
		}

		m_MultiTextures[ i ] = texture;
	}

	Rebuild( );

	return true;
}

bool Pine::Texture3D::SaveToFile( )
{
	if ( !m_Updated )
		return true;

	nlohmann::json j;

	j[ "texture3DType" ] = m_Texture3DType;

	// this is kinda stupid
	static const char* sideLookupTable[ ] = { "Right", "Left", "Top", "Bottom", "Front", "Back" };

	for ( int i = 0; i < 6;i++ )
	{
		if ( m_MultiTextures[ i ] != nullptr )
		{
			j[ sideLookupTable[ i ] ] = m_MultiTextures[ i ]->GetPath( ).string( );
		}
	}

	std::ofstream stream( m_FilePath );

	stream << j;

	stream.close( );

	return true;
}

void Pine::Texture3D::Dispose( )
{
	glDeleteTextures( 1, &m_Id );
}
