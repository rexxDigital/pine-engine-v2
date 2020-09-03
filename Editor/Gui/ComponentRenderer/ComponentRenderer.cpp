#include "ComponentRenderer.hpp"
#include "../Widgets/Widgets.hpp"
#include <Pine/Components/Transform/Transform.hpp>
#include <Pine/Components/ModelRenderer/ModelRenderer.hpp>
#include <Pine/Components/Light/Light.hpp>
#include <Pine/Components/Camera/Camera.hpp>

namespace {

	void RenderTransform(Pine::Transform* transform) {
		if (!transform)
		{
			return;
		}
		
		Gui::Widgets::Vector3("Position", transform->Position);
		Gui::Widgets::Vector3("Rotation", transform->Rotation);
		Gui::Widgets::Vector3("Scale", transform->Scale);
	}

	void RenderModelRenderer(Pine::ModelRenderer* modelRenderer)
	{
		if (!modelRenderer)
		{
			return;
		}
		
		Gui::Widgets::AssetPicker(modelRenderer->GetTargetModel(), "Target Model", Pine::EAssetType::Model);
		
	}
	

}

void Gui::ComponentRenderer::Render(Pine::IComponent* component) {

	switch (component->GetType()) {
	case Pine::EComponentType::Transform:
		RenderTransform(dynamic_cast<Pine::Transform*>(component));
		break;
	case Pine::EComponentType::ModelRenderer:
		RenderModelRenderer(dynamic_cast<Pine::ModelRenderer*>(component));
		break;
	default:
		break;
	}

}
