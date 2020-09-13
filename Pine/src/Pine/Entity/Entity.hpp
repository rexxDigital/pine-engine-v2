#pragma once
#include "../Components/IComponent/IComponent.hpp"
#include "../Components/Transform/Transform.hpp"
#include <stdint.h>
#include <vector>
#include <string>

namespace Pine
{

	class Entity
	{
	private:
		bool m_Active = true;

		// Temporary might be a bad name, used for ie editor entities.
		// stuff that won't get saved by levels etc.
		bool m_IsTemporary = false;

		uint64_t m_Id = 0;
		uint64_t m_EntityIndex = 0;

		std::string m_Name = "Entity";

		std::vector<IComponent*> m_Components;
		std::vector<Entity*> m_Children;

		Entity* m_Parent = nullptr;
	public:
		Entity( uint64_t id );
		Entity( uint64_t id, bool empty );
		~Entity( );

		bool GetActive( ) const;
		void SetActive( bool value );

		bool IsTemporary( ) const;
		void SetTemporary( bool value );
		
		const std::string& GetName( ) const;
		void SetName( const std::string& str );

		uint64_t GetId( ) const;
		uint64_t GetEntityIndex( ) const;

		void SetEntityIndex( uint64_t indx );

		Transform* GetTransform( ) const;

		template<typename T>
		T* GetComponent( )
		{
			for ( auto component : m_Components ) {
				if ( typeid( T ) == typeid( *component ) ) {
					return reinterpret_cast< T* >( component );
				}
			}

			return nullptr;
		}

		void AddComponent( IComponent* component );

		bool RemoveComponent( IComponent* component );
		bool RemoveComponent( int index );

		void ClearComponents( );
		
		const std::vector<IComponent*>& GetComponents( ) const;
		const std::vector<Entity*>& GetChildren( ) const;

		void SetParent( Entity* parent );
		Entity* GetParent( ) const;

		Entity* CreateChild( bool createEntity = true );
		void RemoveChild( Entity* entity );

		// This will unlink and remove all children linked to this entity.
		// So removing them in the entity list as well!
		void DeleteChildren( );

	};

}