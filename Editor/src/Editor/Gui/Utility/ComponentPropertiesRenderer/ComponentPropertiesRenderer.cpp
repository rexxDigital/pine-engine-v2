#include "ComponentPropertiesRenderer.hpp"
#include "..\..\Widgets\Widgets.hpp"

#include <Pine\Components\Transform\Transform.hpp>
#include <Pine\Components\ModelRenderer\ModelRenderer.hpp>
#include <Pine/Components/Camera/Camera.hpp>
#include <Pine/Rendering/RenderManager/RenderManager.hpp>

#include "ImGui/imgui.h"

using namespace Editor::Gui;

namespace {

	void RenderTransform( Pine::Transform* transform ) {
		if ( !transform )
			return;
		
		Widgets::Vector3( "Position", transform->Position );
		Widgets::Vector3( "Rotation", transform->Rotation );
		Widgets::Vector3( "Scale", transform->Scale );
	}

	void RenderModelRenderer( Pine::ModelRenderer* renderer ) {
		if ( !renderer )
			return;

		if ( auto newAsset = Widgets::AssetPicker( "Model", renderer->GetTargetModel( ), true, Pine::EAssetType::Model ) ) {
			renderer->SetTargetModel( reinterpret_cast<Pine::Model*>( newAsset ) );
		}

		if ( auto newAsset = Widgets::AssetPicker( "Override model material", renderer->GetMaterialOverride( ), true, Pine::EAssetType::Material ) ) {
			renderer->SetMaterialOverride( reinterpret_cast< Pine::Material* >( newAsset ) );
		}
	}

	void RenderCamera(Pine::Camera* camera)
	{
		float nearPlane = camera->GetNearPlane();
		float farPlane = camera->GetFarPlane();
		float fieldOfView = camera->GetFieldOfView();

		if (Widgets::SliderFloat("Near Plane", nearPlane, 0.01f, 1.f, "%.2f"))
		{
			camera->SetNearPlane(nearPlane);
		}

		if (Widgets::SliderFloat("Far Plane", farPlane, 50.f, 200.f, "%.1f"))
		{
			camera->SetFarPlane(farPlane);
		}

		if (Widgets::SliderFloat("Field of View", fieldOfView, 30.f, 120.f, "%.1f"))
		{
			camera->SetFieldOfView(fieldOfView);
		}

		bool isActiveCamera = Pine::RenderManager::GetRenderingContext()->m_Camera == camera;

		if (Widgets::Checkbox("Active", isActiveCamera))
		{
			Pine::RenderManager::GetRenderingContext()->m_Camera = isActiveCamera ? camera : nullptr;
		}
	}

}

void Editor::Gui::Utility::ComponentPropertiesRenderer::RenderComponentProperties( Pine::IComponent* component ) {
	
	switch ( component->GetType( ) ) {
	case Pine::EComponentType::Transform:
		RenderTransform( dynamic_cast< Pine::Transform* >( component ) ); break;
	case Pine::EComponentType::ModelRenderer:
		RenderModelRenderer( dynamic_cast< Pine::ModelRenderer* >( component ) ); break;
	case Pine::EComponentType::Camera:
		RenderCamera(dynamic_cast<Pine::Camera*>(component)); break;
	default:
		break;
	}

}
