#include "Serialization.hpp"
#include <filesystem>
#include <fstream>

#include "../../Assets/Assets.hpp"

nlohmann::json Pine::Serialization::LoadJSONFromFile( const std::string& fileName ) {
	if ( !std::filesystem::exists( fileName ) ) {
		return nlohmann::json( );
	}

	std::ifstream stream( fileName );

	if ( !stream.is_open( ) ) {
		return nlohmann::json( );
	}

	nlohmann::json j;
	stream >> j;
	stream.close( );

	return j;
}

glm::vec3 Pine::Serialization::LoadVec3( const nlohmann::json& j, const std::string& name ) {
	try {
		glm::vec3 vec;

		vec.x = j.at( name ).at( "x" );
		vec.y = j.at( name ).at( "y" );
		vec.z = j.at( name ).at( "z" );

		return vec;
	}
	catch ( ... ) {
		return glm::vec3( 0.f, 0.f, 0.f );
	}
}

void Pine::Serialization::SaveVec3( nlohmann::json& j, const glm::vec3& vec )
{
	j[ "x" ] = vec.x;
	j[ "y" ] = vec.y;
	j[ "z" ] = vec.z;
}

void Pine::Serialization::SaveAsset( nlohmann::json& j, IAsset* asset )
{
	// This might get changed in the future, hence it's done this way, in this wrapper.

	if ( !asset )
	{
		j = "null";
		return;
	}

	j = asset->GetPath( ).string( );
}

Pine::IAsset* Pine::Serialization::LoadAsset( const nlohmann::json& j, const std::string& name ) {
	try {
		if ( j.contains( name ) ) {
			if ( j.at( name ) == "null" )
			{
				return nullptr;
			}

			if ( const auto asset = Assets->GetAsset( j.at( name ) ) ) {
				return asset;
			}

			return Assets->LoadFromFile( j.at( name ) );
		}

		return nullptr;
	}
	catch ( ... ) {
		return nullptr;
	}
}
