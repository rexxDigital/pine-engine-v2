#include "Terrain.hpp"
#include "../../OpenGL/VertexArray/VertexArray.hpp"

Pine::TerrainChunk::TerrainChunk( glm::ivec2 pos )
{
	m_Position = pos;
}


const std::string& Pine::TerrainChunk::GetHeightMapFile( ) const
{
	return m_HeightMapFile;
}

const glm::ivec2& Pine::TerrainChunk::GetPosition( ) const
{
	return m_Position;
}

Pine::VertexArray* Pine::TerrainChunk::GetVertexArray( ) const
{
	return m_VertexArray;
}

void Pine::TerrainChunk::GenerateTerrain( )
{
	if ( m_VertexArray )
	{
		m_VertexArray->Dispose( );
		delete m_VertexArray;
	}
	
	const int count = TerrainVertexCount * TerrainVertexCount;

	std::vector<float> vertices( count * 3 );
	std::vector<float> normals( count * 3 );
	std::vector<float> uvs( count * 2 );
	std::vector<int> indices( 6 * ( TerrainVertexCount - 1 ) * ( TerrainVertexCount - 1 ) ); 

	int vertexPointer = 0;
	for ( int i = 0; i < TerrainVertexCount; i++ ) {
		for ( int j = 0; j < TerrainVertexCount; j++ ) {
			vertices[ vertexPointer * 3 ] = static_cast< float >( j ) / ( static_cast< float >( TerrainVertexCount ) - 1 ) * TerrainSize;
			vertices[ vertexPointer * 3 + 1 ] = 0;
			vertices[ vertexPointer * 3 + 2 ] = static_cast< float >( i ) / ( static_cast< float >( TerrainVertexCount ) - 1 ) * TerrainSize;
			normals[ vertexPointer * 3 ] = 0;
			normals[ vertexPointer * 3 + 1 ] = 1;
			normals[ vertexPointer * 3 + 2 ] = 0;
			uvs[ vertexPointer * 2 ] = static_cast< float >( j ) / ( static_cast< float >( TerrainVertexCount ) - 1 );
			uvs[ vertexPointer * 2 + 1 ] = static_cast< float >( i ) / ( static_cast< float >( TerrainVertexCount ) - 1 );
			vertexPointer++;
		}
	}
	
	int pointer = 0;
	for ( int gz = 0; gz < TerrainVertexCount - 1; gz++ ) {
		for ( int gx = 0; gx < TerrainVertexCount - 1; gx++ ) {
			const int topLeft = ( gz * TerrainVertexCount ) + gx;
			const int topRight = topLeft + 1;
			const int bottomLeft = ( ( gz + 1 ) * TerrainVertexCount ) + gx;
			const int bottomRight = bottomLeft + 1;
			
			indices[ pointer++ ] = topLeft;
			indices[ pointer++ ] = bottomLeft;
			indices[ pointer++ ] = topRight;
			indices[ pointer++ ] = topRight;
			indices[ pointer++ ] = bottomLeft;
			indices[ pointer++ ] = bottomRight;
		}
	}

	m_VertexArray = new Pine::VertexArray;
	
	m_VertexArray->Create( );
	m_VertexArray->Bind( );
	
	m_VertexArray->StoreFloatBuffer( vertices, 3, 0 );
	m_VertexArray->StoreFloatBuffer( normals, 3, 1 );
	m_VertexArray->StoreFloatBuffer( uvs, 2, 2 );

	m_VertexArray->StoreIntBuffer( indices );
}

void Pine::TerrainChunk::GenerateTerrain( const std::string& heightMap )
{
	
}

void Pine::TerrainChunk::Dispose( )
{
	if ( m_VertexArray )
		m_VertexArray->Dispose( );

	delete m_VertexArray;
}

Pine::Terrain::Terrain( )
{
	m_Type = EAssetType::Terrain;
}

Pine::TerrainChunk& Pine::Terrain::CreateChunk( glm::ivec2 pos )
{
	m_Chunks.push_back( TerrainChunk( pos ) );

	auto& chunk = m_Chunks[ m_Chunks.size( ) - 1 ];

	chunk.GenerateTerrain( );
	
	return chunk;
}

Pine::TerrainChunk& Pine::Terrain::CreateChunk( glm::ivec2 pos, const std::string& heightMap )
{
	m_Chunks.push_back( TerrainChunk( pos ) );

	auto& chunk = m_Chunks[ m_Chunks.size( ) - 1 ];

	chunk.GenerateTerrain( heightMap );

	return chunk;
}

const std::vector<Pine::TerrainChunk>& Pine::Terrain::GetChunks( ) const
{
	return m_Chunks;
}

bool Pine::Terrain::LoadFromFile( )
{
	return false;
}

bool Pine::Terrain::SaveToFile( )
{
	return false;
}

void Pine::Terrain::Dispose( )
{
	for ( auto& chunk : m_Chunks )
	{
		chunk.Dispose( );
	}
}
