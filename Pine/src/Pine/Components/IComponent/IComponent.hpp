#pragma once

namespace Pine
{

	enum class EComponentType
	{
		Invalid,
		Transform,
		ModelRenderer,
		Camera,
		Light
	};

	class Entity;

	class IComponent
	{
	protected:
		bool m_Active = true;
		
		EComponentType m_ComponentType = EComponentType::Invalid;
		Pine::Entity* m_Parent = nullptr;
	public:
		virtual ~IComponent() = default;

		EComponentType GetType() const;

		void SetParent(Pine::Entity* parent);
		Pine::Entity* GetParent();

		const bool GetActive() const;
		void SetActive(bool value);

		virtual void OnSetup() = 0;
		virtual void OnUpdate(float deltaTime) = 0;
		virtual void OnRender();
		virtual void OnDestroy();
	};

}