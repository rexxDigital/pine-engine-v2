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

		void CopyEntity( Pine::Entity* target, const Pine::Entity* entity, bool createInstance ) const;

	public:
		Blueprint( );
		
		void CreateFromEntity( const Pine::Entity* entity );

		Pine::Entity* SpawnEntity( ) const;
		bool HasValidEntity( ) const;

		Pine::Entity* GetInternalEntity( ) const;

		nlohmann::json ToJson( ) const;
		bool FromJson( nlohmann::json& j );
		
		bool LoadFromFile( ) override;
		bool SaveToFile( ) override;
		void Dispose( ) override;
		
	};
	
}
