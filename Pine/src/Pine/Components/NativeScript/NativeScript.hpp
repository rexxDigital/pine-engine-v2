#pragma once
#include "../IComponent/IComponent.hpp"

namespace Pine
{

	class NativeScript : public IComponent
	{
	protected:

		bool m_CreateFromFactory = false;
		std::string m_FactoryName = "";

	public:
		NativeScript( );

		void SetCreateFromFactory( bool value );
		bool GetCreateFromFactory( ) const;

		void OnSetup( ) override;
	};

}
