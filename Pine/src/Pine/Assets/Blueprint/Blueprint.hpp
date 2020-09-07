#pragma once

#include "../IAsset/IAsset.hpp"

namespace Pine
{
	class Entity;

	class Blueprint : public IAsset
	{
	private:

		Pine::Entity* m_Entity = nullptr;

		void SaveEntity( Pine::Entity* target, Pine::Entity* entity );
		
	public:
		void CreateFromEntity( Pine::Entity* entity );

		void SpawnEntity( ) const;
		bool HasValidEntity( ) const;
		
		bool LoadFromFile( ) override;
		bool SaveToFile( ) override;
		void Dispose( ) override;
		
	};
	
}
