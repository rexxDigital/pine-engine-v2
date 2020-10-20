#include "ComponentPropertiesRenderer.hpp"
#include <Pine\Components\Transform\Transform.hpp>

namespace {

	void RenderTransform( Pine::Transform* transform ) {

	}

}

void Editor::Gui::Utility::ComponentPropertiesRenderer::RenderComponentProperties( Pine::IComponent* component ) {
	
	switch ( component->GetType( ) ) {
	case Pine::EComponentType::Transform:
		RenderTransform( dynamic_cast< Pine::Transform* >( component ) ); break;

	default:
		break;
	}

}
