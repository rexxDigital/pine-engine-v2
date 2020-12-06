#pragma once
#include "../IComponent/IComponent.hpp"
#include "../../Assets/Terrain/Terrain.hpp"

namespace Pine
{

	class TerrainRenderer : public IComponent
	{
	private:
		Pine::Terrain* m_Terrain = nullptr;
	public:
		TerrainRenderer( );

		void SetTerrain( Pine::Terrain* terrain );
		Pine::Terrain* GetTerrain( ) const;

		void OnSetup( ) override;
		void OnUpdate( float deltaTime ) override;
		void SaveToJson( nlohmann::json& j ) override;
		void LoadFromJson( nlohmann::json& j ) override;

		IComponent* Clone( ) override;
	};

}