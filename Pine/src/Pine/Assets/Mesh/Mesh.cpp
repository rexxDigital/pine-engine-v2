#include "Mesh.hpp"
#include "../Model/Model.hpp"

Pine::Mesh::Mesh( Model* model )
{
	m_VertexArray = std::make_unique<VertexArray>( );
	m_VertexArray->Create( );

	m_Material = Assets->GetAsset<Pine::Material>( "Assets\\Engine\\Materials\\Default.mat" );

	m_ParentModel = model;
}

Pine::Model* Pine::Mesh::GetParentModel( ) const
{
	return m_ParentModel;
}

Pine::Material* Pine::Mesh::GetMaterial( ) const
{
	return m_Material.Get( );
}

void Pine::Mesh::SetMaterial( Material* material )
{
	m_Material = material;
	m_ParentModel->SetUpdated( true );
}

Pine::VertexArray* Pine::Mesh::GetVertexArray( ) const
{
	return m_VertexArray.get( );
}

uint64_t Pine::Mesh::GetRenderCount( ) const
{
	return m_RenderCount;
}

bool Pine::Mesh::HasElementBuffer( ) const
{
	return m_HasElementBuffer;
}

const glm::vec3& Pine::Mesh::GetMins( ) const
{
	return m_Mins;
}

const glm::vec3& Pine::Mesh::GetMaxs( ) const
{
	return m_Maxs;
}

void Pine::Mesh::SetMaxs( glm::vec3 aabb )
{
	m_Mins = aabb;
}

void Pine::Mesh::SetMins( glm::vec3 aabb )
{
	m_Maxs = aabb;
}

void Pine::Mesh::SetVertices( const std::vector<float>& vertices )
{
	m_VertexArray->Bind( );
	m_VertexArray->StoreFloatBuffer( vertices, 3, Internal::VERTEX_ARRAY_BUFFER );
	m_RenderCount = vertices.size( );
}

void Pine::Mesh::SetIndices( const std::vector<int>& indices )
{
	m_VertexArray->Bind( );
	m_VertexArray->StoreIntBuffer( indices );
	m_RenderCount = indices.size( );
	m_HasElementBuffer = true;
}

void Pine::Mesh::SetNormals( const std::vector<float>& normals )
{
	m_VertexArray->Bind( );
	m_VertexArray->StoreFloatBuffer( normals, 3, Internal::NORMAL_ARRAY_BUFFER );
}

void Pine::Mesh::SetUvs( const std::vector<float>& uvs )
{
	m_VertexArray->Bind( );
	m_VertexArray->StoreFloatBuffer( uvs, 2, Internal::UV_ARRAY_BUFFER );
}

void Pine::Mesh::Dispose( )
{
	m_VertexArray->Dispose( );
}
