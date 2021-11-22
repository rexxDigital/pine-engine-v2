#include "TerrainRenderer.hpp"

#include "../../Core/Serialization/Serialization.hpp"

Pine::TerrainRenderer::TerrainRenderer( )
{
	m_ComponentType = EComponentType::TerrainRenderer;
}

void Pine::TerrainRenderer::SetTerrain( Pine::Terrain* terrain )
{
	m_Terrain = terrain;
}

Pine::Terrain* Pine::TerrainRenderer::GetTerrain( ) const
{
	return m_Terrain;
}

void Pine::TerrainRenderer::OnSetup( )
{
}

void Pine::TerrainRenderer::OnUpdate( float deltaTime )
{
}

void Pine::TerrainRenderer::SaveToJson( nlohmann::json& j )
{
	Serialization::SaveAsset( j[ "terrain" ], m_Terrain );
}

void Pine::TerrainRenderer::LoadFromJson( nlohmann::json& j )
{
	m_Terrain = dynamic_cast< Pine::Terrain* >( Serialization::LoadAsset( j, "terrain" ) );
}

