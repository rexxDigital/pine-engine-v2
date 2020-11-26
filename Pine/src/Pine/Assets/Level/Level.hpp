#pragma once
#include "../Blueprint/Blueprint.hpp"
#include "../IAsset/IAsset.hpp"

namespace Pine
{

	struct LevelSettings
	{
		uint64_t m_CameraEntity = 0;
	};
	
	class Level : public IAsset
	{
	private:
		std::vector<Blueprint*> m_Blueprints;

		void DisposeBlueprints( );
		Blueprint* CreateBlueprintOfEntity( Pine::Entity* entity );

		LevelSettings* m_LevelSettings = nullptr;
	public:
		Level( );

		int GetBlueprintCount( ) const;

		void CreateFromCurrentLevel( );
		void Load( );

		LevelSettings* GetSettings( ) const;
		
		bool LoadFromFile( ) override;
		bool SaveToFile( ) override;
		void Dispose( ) override;
	};

}