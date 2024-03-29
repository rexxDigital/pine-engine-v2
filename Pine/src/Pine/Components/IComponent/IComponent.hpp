#pragma once
#include <json.hpp>

namespace Pine
{

	enum class ComponentType
	{
		Invalid,
		Transform,
		ModelRenderer,
		Camera,
		Light,
		NativeScript,
		TerrainRenderer,
		Collider3D,
		RigidBody
	};

	inline const char* SComponentNames[ ] = {
		"Invalid",
		"Transform",
		"Model Renderer",
		"Camera",
		"Light",
		"Native Script",
		"Terrain Renderer",
		"Collider3D",
		"Rigid Body"
	};

	class Entity;

	class IComponent
	{
	protected:
		bool m_Active = true;
		bool m_Standalone = true;

		ComponentType m_ComponentType = ComponentType::Invalid;
		Entity* m_Parent = nullptr;
	public:
		virtual ~IComponent( ) = default;

		ComponentType GetType( ) const;

		void SetParent( Pine::Entity* parent );
		Entity* GetParent( ) const;

		bool GetActive( ) const;
		void SetActive( bool value );

		bool GetStandalone( ) const;
		void SetStandalone( bool value );

		virtual void OnSetup( ) = 0;
		virtual void OnUpdate( float deltaTime ) = 0;
		virtual void OnRender( );
		virtual void OnRenderUI( );

		virtual void OnCreated( );
		virtual void OnCopied( const IComponent* old );
		virtual void OnDestroyed( );

		virtual void LoadFromJson( nlohmann::json& j );
		virtual void SaveToJson( nlohmann::json& j );
	};

}
