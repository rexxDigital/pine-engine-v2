#include "ComponentRenderer.hpp"
#include "../Widgets/Widgets.hpp"
#include <Pine/Components/Transform/Transform.hpp>

namespace {

	void RenderTransform(Pine::Transform* transform) {

		Gui::Widgets::Vector3("Position", transform->Position);
		Gui::Widgets::Vector3("Rotation", transform->Rotation);
		Gui::Widgets::Vector3("Scale", transform->Scale);
		
	}


}

void Gui::ComponentRenderer::Render(Pine::IComponent* component) {

	switch (component->GetType()) {
	case Pine::EComponentType::Transform:
		RenderTransform(dynamic_cast<Pine::Transform*>(component));
		break;
	default:
		break;
	}

}
