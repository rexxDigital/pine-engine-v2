#pragma once
#include "../IComponent/IComponent.hpp"

namespace Pine
{

	class NativeScript : public IComponent
	{
	protected:

		bool m_CreateFromFactory = true;
		std::string m_FactoryName = "";

		IComponent* m_InternalComponent = nullptr;

		void CreateInternalComponent( );

	public:
		NativeScript( );

		void SetCreateFromFactory( bool value );
		bool GetCreateFromFactory( ) const;

		void SetFactoryName( const std::string& str );
		const std::string& GetFactoryName( ) const;

		void OnSetup( ) override;
		void OnRender( ) override;
		void OnUpdate( float deltaTime ) override;

		void OnDestroyed( ) override;
	};

}
