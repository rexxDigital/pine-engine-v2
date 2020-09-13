#pragma once
#include "../Blueprint/Blueprint.hpp"
#include "../IAsset/IAsset.hpp"

namespace Pine
{

	class Level : public IAsset
	{
	private:
		std::vector<Blueprint*> m_Blueprints;

		void DisposeBlueprints( );
		Blueprint* CreateBlueprintOfEntity( Pine::Entity* entity );
	public:
		Level( );

		int GetBlueprintCount( ) const;

		void CreateFromCurrentLevel( );
		void Load( );

		bool LoadFromFile( ) override;
		bool SaveToFile( ) override;
		void Dispose( ) override;
	};

}