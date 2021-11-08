#pragma once
#include <vector>
#include "../Entity/Entity.hpp"
#include "../Core/Interfaces/Interfaces.hpp"

namespace Pine
{

	class IEntityList : public IInterface
	{
	public:

		virtual Entity* CreateEntity( ) = 0;
		virtual Entity* CreateEntity( const std::string& name ) = 0;

		virtual void ClearEntities( bool temp = false ) = 0;
		virtual	bool DeleteEntity( Entity* entity ) = 0;

		virtual void MoveEntity( Entity* entity, int newPosition ) = 0;

		virtual void RunOnSetup( ) = 0;
		virtual void RunOnUpdate( float deltaTime ) = 0;

		virtual const std::vector<Entity>& GetEntities( ) = 0;
		virtual Entity* GetEntity( const int index ) = 0;

	};

}
