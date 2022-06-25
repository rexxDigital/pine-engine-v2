#pragma once
#include "../IComponent/IComponent.hpp"

namespace Pine
{

	class NativeScript : public IComponent
	{
	protected:

        std::string m_FactoryName;
		bool m_CreateFromFactory = true;

		IComponent* m_InternalComponent = nullptr;

		void SetupInternalComponent( );
		void CreateInternalComponent( );

	public:
		NativeScript( );

		void SetCreateFromFactory( bool value );
		bool GetCreateFromFactory( ) const;

		void SetFactoryName( const std::string& str );
		const std::string& GetFactoryName( ) const;

		void SetInternalComponent( IComponent* component );
		IComponent* GetInternalComponent( ) const;

		void OnSetup( ) override;
		void OnRender( ) override;
		void OnUpdate( float deltaTime ) override;

		void LoadFromJson( nlohmann::json& j ) override;
		void SaveToJson( nlohmann::json& j ) override;

		void OnCreated( ) override;
		void OnDestroyed( ) override;
	};

}
