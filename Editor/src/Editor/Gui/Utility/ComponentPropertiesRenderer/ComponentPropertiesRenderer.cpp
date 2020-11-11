#include "ComponentPropertiesRenderer.hpp"
#include <Pine\Components\Transform\Transform.hpp>
#include "..\..\Widgets\Widgets.hpp"

using namespace Editor::Gui;

namespace {

	void RenderTransform( Pine::Transform* transform ) {
		Widgets::Vector3( "Position", transform->Position );
		Widgets::Vector3( "Rotation", transform->Rotation );
		Widgets::Vector3( "Scale", transform->Scale );
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
