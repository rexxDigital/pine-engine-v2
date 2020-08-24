#pragma once
#include <vector>
#include "../Entity/Entity.hpp"

namespace Pine::EntityList {
	
	Entity* CreateEntity();
	Entity* CreateEntity(const std::string& name);

	bool DeleteEntity(Entity* entity);

	const std::vector<Entity*>& GetEntities();

	void RunOnSetup();

}