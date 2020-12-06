#pragma once
#include <glm/vec2.hpp>

#include "../IAsset/IAsset.hpp"

namespace Pine {

	constexpr int TerrainVertexCount = 128;
	constexpr int TerrainSize = 800;

	class VertexArray;
	class TerrainChunk
	{
	private:
		glm::ivec2 m_Position;

		std::string m_HeightMapFile = "";
		
		VertexArray* m_VertexArray = nullptr;
	public:
		TerrainChunk( glm::ivec2 pos );

		const std::string& GetHeightMapFile( ) const;
		const glm::ivec2& GetPosition( ) const;
		VertexArray* GetVertexArray( ) const;
		
		void GenerateTerrain( );
		void GenerateTerrain( const std::string& heightMap );

		void Dispose( );
	};

	class Terrain : public IAsset
	{
	private:
		std::vector<TerrainChunk> m_Chunks;
	public:
		Terrain( );

		TerrainChunk& CreateChunk( glm::ivec2 pos );
		TerrainChunk& CreateChunk( glm::ivec2 pos, const std::string& heightMap );

		const std::vector<TerrainChunk>& GetChunks( ) const;
		
		bool LoadFromFile( ) override;
		bool SaveToFile( ) override;
		void Dispose( ) override;
	};

}
