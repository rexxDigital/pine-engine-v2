#pragma once
#include <vector>
#include "../Entity/Entity.hpp"

namespace Pine::EntityList {

	void Setup( );

	Entity* CreateEntity( );
	Entity* CreateEntity( const std::string& name );

	void ClearEntities( bool temp = false );
	bool DeleteEntity( Entity* entity );

	void MoveEntity( Entity* entity, int newPosition );

	void RunOnSetup( );
	void RunOnUpdate( float deltaTime );

	std::vector<Entity>& GetEntities( );
	
}