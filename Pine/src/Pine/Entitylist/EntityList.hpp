#pragma once
#include <vector>
#include "../Entity/Entity.hpp"

namespace Pine::EntityList {

	Entity* CreateEntity( );
	Entity* CreateEntity( const std::string& name );

	void ClearEntities( bool temp = false );
	bool DeleteEntity( Entity* entity );

	void RunOnSetup( );
	void RunOnUpdate( float deltaTime );

	const std::vector<Entity*>& GetEntities( );
	
}