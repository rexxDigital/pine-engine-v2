#pragma once
#include "../Components/IComponent/IComponent.hpp"
#include "../Components/Transform/Transform.hpp"
#include <stdint.h>
#include <vector>
#include <string>

namespace Pine
{
	// Used to sort entities, making the engine run a bit faster.
	// This is managed internally by the engine, so you don't need to worry about this.
	enum class EEntityFlags {
		Renderable = (1 << 0),
		Camera = (1 << 1),
		Light = (1 << 2),
		Scriptable = (1 << 3),
	};

	class Entity 
	{
	private:
		bool m_Active = true;

		uint64_t m_Id = 0;
		uint64_t m_EntityIndex = 0;
		uint64_t m_EntityFlags = 0;

		std::string m_Name = "Entity";

		std::vector<IComponent*> m_Components;
		std::vector<Entity*> m_Children;

		Entity* m_Parent = nullptr;
	public:
		Entity(uint64_t id);
		~Entity();

		bool GetActive() const;
		void SetActive(bool value);

		const std::string& GetName() const;
		void SetName(const std::string& str);

		uint64_t GetId() const;
		uint64_t GetEntityIndex() const;

		uint64_t& GetEntityFlags();

		void SetEntityIndex(uint64_t indx);

		Transform* GetTransform() const;

		template<typename T>
		T* GetComponent() 
		{
			for (auto component : m_Components) {
				if (typeid(T) == typeid(*component)) {
					return reinterpret_cast<T*>(component);
				}
			}

			return nullptr;
		}

		void AddComponent(IComponent* component);

		const std::vector<IComponent*>& GetComponents() const;

		const std::vector<Entity*>& GetChildren() const;

		void SetParent(Entity* parent);
		Entity* GetParent() const;
		
		Entity* CreateChild();
		void RemoveChild(Entity* entity);

		// This will unlink and remove all children linked to this entity.
		// So removing them in the entity list as well!
		void DeleteChildren();
		
	};

}