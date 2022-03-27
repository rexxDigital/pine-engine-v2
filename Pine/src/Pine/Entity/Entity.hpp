#pragma once
#include "../Components/IComponent/IComponent.hpp"
#include "../Components/Transform/Transform.hpp"
#include "../Components/NativeScript/NativeScript.hpp"
#include <stdint.h>
#include <vector>
#include <string>

namespace Pine
{

	class Entity
	{
	private:
		bool m_Active = true;
        bool m_Static = false;

		// Temporary might be a bad name, used for example editor entities.
		// So stuff that won't get saved by levels etc.
		bool m_IsTemporary = false;

		uint64_t m_Id = 0;
		int m_EntityIndex = 0;

		std::string m_Name = "Entity";

		std::vector<IComponent*> m_Components;
		std::vector<Entity*> m_Children;

		Entity* m_Parent = nullptr;
	public:
		Entity( uint64_t id );
		Entity( uint64_t id, bool empty );
		~Entity( );

		uint64_t GetId( ) const;

		bool GetActive( ) const;
		void SetActive( bool value );

        bool GetStatic( ) const;
        void SetStatic( bool value );

		void SetEntityIndex( int indx );
        int GetEntityIndex( ) const;

		bool IsTemporary( ) const;
		void SetTemporary( bool value );

		const std::string& GetName( ) const;
		void SetName( const std::string& str );

		Transform* GetTransform( ) const;

		template<typename T>
		T* GetComponent( )
		{
			for ( auto component : m_Components )
			{
				if ( component && typeid( T ) == typeid( *component ) ) // god bless RTTI
				{
					return reinterpret_cast< T* >( component );
				}
			}

			return nullptr;
		}

		template<typename T>
		T* GetScript( )
		{
			for ( const auto component : m_Components )
			{
				if ( component->GetType( ) == ComponentType::NativeScript )
				{
					auto internalComponent = dynamic_cast< NativeScript* >( component )->GetInternalComponent( );

					if ( typeid( T ) == typeid( *internalComponent ) ) // god bless RTTI
					{
						return reinterpret_cast< T* >( internalComponent );
					}
				}
			}

			return nullptr;
		}

		void AddScript( IComponent* script );
		void AddScript( const std::string& name );

		void AddComponent( ComponentType type );

		// Please use the other AddComponent instead when you can as this completely bypasses the "ECS" system.
		// This is used internally or when you have a "custom" component. 
		void AddComponent( IComponent* component );

		bool RemoveComponent( IComponent* component );
		bool RemoveComponent( int index );

		void ClearComponents( );

		const std::vector<IComponent*>& GetComponents( ) const;
		const std::vector<Entity*>& GetChildren( ) const;

		void SetParent( Entity* parent );
		Entity* GetParent( ) const;

		Entity* CreateChild( bool createEntity = true );
		void AddChild( Entity* entity );
		void RemoveChild( Entity* entity );

		// This will unlink and remove all children linked to this entity.
		// And it will remove them in the entity list as well!
		void DeleteChildren( );

		void Dispose( );

	};

}