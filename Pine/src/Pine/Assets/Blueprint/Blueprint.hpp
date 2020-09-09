#pragma once

#include <json.hpp>

#include "../IAsset/IAsset.hpp"

namespace Pine
{
	class Entity;

	class Blueprint : public IAsset
	{
	protected:

		Pine::Entity* m_Entity = nullptr;

		void CopyEntity( Pine::Entity* target, const Pine::Entity* entity ) const;

	public:
		Blueprint( );
		
		void CreateFromEntity( Pine::Entity* entity );

		void SpawnEntity( ) const;
		bool HasValidEntity( ) const;
		nlohmann::json ToJson( ) const;
		
		bool LoadFromFile( ) override;
		bool SaveToFile( ) override;
		void Dispose( ) override;
		
	};
	
}
