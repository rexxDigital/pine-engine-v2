#include "Model.hpp"

#include <filesystem>
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../../Core/Log/Log.hpp"
#include "../../Core/Serialization/Serialization.hpp"
#include "../../OpenGL/FrameBuffer/FrameBuffer.hpp"
#include "../../Rendering/Renderer3D/Renderer3D.hpp"

namespace {

	std::string currentModelFile = "";

	void ProcessMesh( Pine::Mesh* eMesh, aiMesh* mesh, const aiScene* scene ) {
		// Fill all the vectors below with the data imported from ASSIMP
		std::vector<float> vertices;
		std::vector<float> normals;
		std::vector<float> uvs;
		std::vector<int> indices;

		// Loop through all ASSIMP vertices, and add them into the vectors
		for ( unsigned int i = 0; i < mesh->mNumVertices; i++ ) {
			vertices.push_back( mesh->mVertices[ i ].x );
			vertices.push_back( mesh->mVertices[ i ].y );
			vertices.push_back( mesh->mVertices[ i ].z );

			normals.push_back( mesh->mNormals[ i ].x );
			normals.push_back( mesh->mNormals[ i ].y );
			normals.push_back( mesh->mNormals[ i ].z );

			if ( mesh->HasTextureCoords( 0 ) ) {
				uvs.push_back( mesh->mTextureCoords[ 0 ][ i ].x );
				uvs.push_back( mesh->mTextureCoords[ 0 ][ i ].y );
			}
			else {
				// Because we still need to have a UV coordinate within the vector at any vertex position.
				uvs.push_back( 0.f );
				uvs.push_back( 0.f );
			}
		}

		// Fill out the indices vector
		for ( unsigned int i = 0; i < mesh->mNumFaces; i++ ) {
			aiFace face = mesh->mFaces[ i ];
			for ( unsigned int j = 0; j < face.mNumIndices; j++ )
				indices.push_back( face.mIndices[ j ] );

		}

		// Load the engine's Mesh class with the data.
		eMesh->GetVertexArray( )->Bind( );

		eMesh->SetVertices( vertices );
		eMesh->SetNormals( normals );
		eMesh->SetIndices( indices );
		eMesh->SetUvs( uvs );

		// Setup generated AABB
		const auto min = mesh->mAABB.mMin;
		const auto max = mesh->mAABB.mMax;

		eMesh->SetMins( glm::vec3( min.x, min.y, min.z ) );
		eMesh->SetMaxs( glm::vec3( max.x, max.y, max.z ) );

		// Process all the materials loaded by ASSIMP, and then assign them to the engine's mesh material.
		auto material = scene->mMaterials[ mesh->mMaterialIndex ];
		// Make sure the material is valid, and that it's not the automatically generated default material.
		if ( material && strcmp( material->GetName( ).C_Str( ), AI_DEFAULT_MATERIAL_NAME ) != 0 ) {
			eMesh->SetMaterial( new Pine::Material( ) );

			auto eMaterial = eMesh->GetMaterial( );

			// get material colors:
			aiColor3D diffuse_color( 1.f, 1.f, 1.f );
			aiColor3D ambient_color( 1.f, 1.f, 1.f );
			aiColor3D specular_color( 0.f, 0.f, 0.f );

			float shininess = 1.f;

			material->Get( AI_MATKEY_COLOR_DIFFUSE, diffuse_color );
			material->Get( AI_MATKEY_COLOR_SPECULAR, specular_color );
			material->Get( AI_MATKEY_COLOR_AMBIENT, ambient_color );
			material->Get( AI_MATKEY_SHININESS, shininess );

			eMaterial->DiffuseColor( ) = glm::vec3( diffuse_color.r, diffuse_color.g, diffuse_color.b );
			eMaterial->AmbientColor( ) = glm::vec3( ambient_color.r, ambient_color.g, ambient_color.b );
			eMaterial->SpecularColor( ) = glm::vec3( specular_color.r, specular_color.g, specular_color.b );

			eMaterial->SetShininiess( shininess );

			auto parentDir = eMesh->GetParentModel( )->GetPath( ).parent_path( ).string( );

			// process material textures
			if ( material->GetTextureCount( aiTextureType_DIFFUSE ) > 0 ) {
				aiString file_path;
				material->GetTexture( aiTextureType_DIFFUSE, 0, &file_path );

				eMaterial->SetDiffuse( reinterpret_cast< Pine::Texture2D* >( Pine::Assets::LoadFromFile( parentDir + "\\" + file_path.C_Str( ) ) ) );
			}

			//if ( material->GetTextureCount( aiTextureType_SPECULAR ) > 0 ) {
			//	aiString file_path;
			//	material->GetTexture( aiTextureType_SPECULAR, 0, &file_path );
			//	eMaterial->SetSpecular( reinterpret_cast< Pine::Texture2D* >( Pine::Assets::LoadFromFile( parentDir + "\\" + file_path.C_Str( ) ) ) );
			//}

			eMaterial->SetGenerated( true );
		}

	}

	void ProcessNode( Pine::Model* mdl, aiNode* node, const aiScene* scene ) {
		// Loop through all the meshes within the model
		for ( int i = 0; i < node->mNumMeshes; i++ ) {
			const auto mesh = scene->mMeshes[ node->mMeshes[ i ] ]; // Retrieve the ASSIMOP mesh class
			const auto engineMesh = mdl->CreateMesh( ); // Create a new instance of the Mesh class within the engine

			ProcessMesh( engineMesh, mesh, scene );
		}

		// Process additional nodes via the magic of recursion
		for ( int i = 0; i < node->mNumChildren; i++ ) {
			ProcessNode( mdl, node->mChildren[ i ], scene );
		}
	}



}

Pine::Model::Model( ) {
	m_Type = EAssetType::Model;
}

Pine::Mesh* Pine::Model::CreateMesh( ) {
	auto mesh = new Pine::Mesh( this );

	m_MeshList.push_back( mesh );

	return mesh;
}

const std::vector<Pine::Mesh*>& Pine::Model::GetMeshList( ) const {
	return m_MeshList;
}

bool Pine::Model::LoadFromFile( ) {

	// Load the model file into a aiScene object, where we can later on read the data
	Assimp::Importer importer;
	const auto scene = importer.ReadFile( m_FilePath.string( ), aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_GenSmoothNormals | aiProcess_GenBoundingBoxes );

	if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) {
		Log::Error( "Assimp loading error " + m_FilePath.string( ) + ", " + importer.GetErrorString( ) );
		return false;
	}

	ProcessNode( this, scene->mRootNode, scene );

	// Load custom materials if we have any:
	const auto assetFilePath = m_FilePath.parent_path( ).string( ) + "\\" + m_FilePath.filename( ).string( ) + ".asset";

	if ( std::filesystem::exists( assetFilePath ) )
	{
		try
		{
			auto jsonObject = Pine::Serialization::LoadJSONFromFile( assetFilePath );

			for ( int i = 0; i < m_MeshList.size( ); i++ )
			{
				const auto meshStr = std::to_string( i );

				if ( !jsonObject.contains( meshStr ) )
				{
					continue;
				}

				if ( jsonObject[ meshStr ].contains( "mat" ) )
				{
					if ( !std::filesystem::exists( jsonObject[ meshStr ][ "mat" ].get<std::string>(  ) ) )
					{
						Log::Warning( "Could not load mesh material file " + jsonObject[ meshStr ][ "mat" ].get<std::string>(  ) + ", file doesn't exist." );
						continue;
					}
					
					const auto materialAsset = dynamic_cast< Pine::Material* >( Pine::Assets::LoadFromFile( jsonObject[ meshStr ][ "mat" ] ) );

					if ( materialAsset == nullptr ) // weird but ok
					{
						Log::Warning( "Failed to find material file from mesh" );
						continue;
					}

					m_MeshList[ i ]->SetMaterial( materialAsset );
				}
			}
		}
		catch ( std::exception& e )
		{
			Log::Error( "Failed to parse model asset json, " + std::string( e.what( ) ) );
		}

	}

	return true;
}

bool Pine::Model::SaveToFile( ) {
	nlohmann::json j;
	bool wroteData = false;

	const auto assetFilePath = m_FilePath.parent_path( ).string( ) + "\\" + m_FilePath.filename( ).string( ) + ".asset";

	// Check if we have any custom materials in place.
	for ( int i = 0; i < m_MeshList.size( ); i++ )
	{
		const auto mesh = m_MeshList[ i ];

		if ( mesh->GetMaterial( )->GetReadOnly( ) || mesh->GetMaterial( )->IsGenerated( ) )
		{
			continue;
		}

		wroteData = true;
		j[ std::to_string( i ) ][ "mat" ] = mesh->GetMaterial( )->GetPath( ).string( );
	}

	if ( std::filesystem::exists( assetFilePath ) )
		std::filesystem::remove( assetFilePath );

	if ( !wroteData )
	{
		return true;
	}

	std::ofstream stream( assetFilePath );

	stream << j;

	stream.close( );

	return true;
}

void Pine::Model::Dispose( ) {
	for ( auto mesh : m_MeshList ) {
		mesh->Dispose( );
		delete mesh;
	}
}
