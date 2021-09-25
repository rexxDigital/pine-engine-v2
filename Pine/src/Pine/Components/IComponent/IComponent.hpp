#pragma once
#include <json.hpp>

namespace Pine
{

	enum class EComponentType
	{
		Invalid,
		Transform,
		ModelRenderer,
		Camera,
		Light,
		NativeScript,
		Behavior,
		TerrainRenderer,
		Collider
	};

	inline const char* SComponentNames[ ] = {
		"Invalid",
		"Transform",
		"Model Renderer",
		"Camera",
		"Light",
		"Native Script",
		"Behavior",
		"Terrain Renderer",
		"Collider"
	};

	class Entity;

	class IComponent
	{
	protected:
		bool m_Active = true;
		bool m_Standalone = true;

		EComponentType m_ComponentType = EComponentType::Invalid;
		Pine::Entity* m_Parent = nullptr;
	public:
		virtual ~IComponent( ) = default;

		EComponentType GetType( ) const;

		void SetParent( Pine::Entity* parent );
		Pine::Entity* GetParent( ) const;

		bool GetActive( ) const;
		void SetActive( bool value );

		bool GetStandalone( ) const;
		void SetStandalone( bool value );

		virtual void OnSetup( ) = 0;
		virtual void OnUpdate( float deltaTime ) = 0;
		virtual void OnRender( );

		virtual void OnCreated( );
		virtual void OnDestroyed( );

		virtual void LoadFromJson( nlohmann::json& j );
		virtual void SaveToJson( nlohmann::json& j );
	};

}
