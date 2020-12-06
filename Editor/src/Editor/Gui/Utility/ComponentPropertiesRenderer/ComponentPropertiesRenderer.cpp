#include "ComponentPropertiesRenderer.hpp"
#include "..\..\Widgets\Widgets.hpp"

#include <Pine\Components\Transform\Transform.hpp>
#include <Pine\Components\ModelRenderer\ModelRenderer.hpp>
#include <Pine/Components/Camera/Camera.hpp>
#include <Pine/Rendering/RenderManager/RenderManager.hpp>

#include <Pine/Assets/Script/Script.hpp>

#include "ImGui/imgui.h"
#include "Pine/Components/Behavior/Behavior.hpp"
#include "Pine/Components/Light/Light.hpp"
#include "Pine/Components/TerrainRenderer/TerrainRenderer.hpp"

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
			renderer->SetTargetModel( reinterpret_cast< Pine::Model* >( newAsset ) );
		}

		if ( auto newAsset = Widgets::AssetPicker( "Override model material", renderer->GetMaterialOverride( ), true, Pine::EAssetType::Material ) ) {
			renderer->SetMaterialOverride( reinterpret_cast< Pine::Material* >( newAsset ) );
		}
	}

	void RenderCamera( Pine::Camera* camera )
	{
		float nearPlane = camera->GetNearPlane( );
		float farPlane = camera->GetFarPlane( );
		float fieldOfView = camera->GetFieldOfView( );
		bool isActiveCamera = Pine::RenderManager::GetRenderingContext( )->m_Camera == camera;

		if ( Widgets::Checkbox( "Active", isActiveCamera ) )
		{
			Pine::RenderManager::GetRenderingContext( )->m_Camera = isActiveCamera ? camera : nullptr;
		}

		if ( Widgets::SliderFloat( "Near Plane", nearPlane, 0.01f, 1.f, "%.2f" ) )
		{
			camera->SetNearPlane( nearPlane );
		}

		if ( Widgets::SliderFloat( "Far Plane", farPlane, 50.f, 200.f, "%.1f" ) )
		{
			camera->SetFarPlane( farPlane );
		}

		if ( Widgets::SliderFloat( "Field of View", fieldOfView, 30.f, 120.f, "%.1f" ) )
		{
			camera->SetFieldOfView( fieldOfView );
		}
	}

	void RenderLight( Pine::Light* light )
	{
		if ( !light )
			return;

		int lightType = static_cast< int >( light->GetLightType( ) );
		if ( Widgets::Combobox( "Type", lightType, "Directional\0Point light" ) )
		{
			light->SetLightType( static_cast< Pine::ELightType >( lightType ) );
		}

		auto lightColor = light->GetLightColor( );
		if ( Widgets::ColorPicker( "Light Color", lightColor ) )
		{
			light->SetLightColor( lightColor );
		}

		auto attenuation = light->GetAttenuation( );

		if ( Widgets::SliderFloat( "Linear", attenuation.y, 0.0001f, 1.f, "%.5f" ) )
		{
			light->SetAttenuation( attenuation );
		}

		if ( Widgets::SliderFloat( "Quadratic", attenuation.z, 0.0001f, 2.f, "%.5f" ) )
		{
			light->SetAttenuation( attenuation );
		}
 	}

	void RenderBehavior( Pine::Behavior* behavior )
	{
		const auto currentAsset = behavior->GetAttachedScript( );
		if ( const auto newAsset = Widgets::AssetPicker( "Script", currentAsset, true, Pine::EAssetType::Script ) )
		{
			behavior->SetAttachedScript( dynamic_cast< Pine::Script* >( newAsset ) );
		}
	}

	void RenderTerrainRenderer( Pine::TerrainRenderer* terrainRenderer )
	{
		if ( !terrainRenderer )
			return;

		const auto currentAsset = terrainRenderer->GetTerrain( );
		if ( const auto newAsset = Widgets::AssetPicker( "Terrain", currentAsset, true, Pine::EAssetType::Terrain ) )
		{
			terrainRenderer->SetTerrain( dynamic_cast< Pine::Terrain* >( newAsset ) );
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
		RenderCamera( dynamic_cast< Pine::Camera* >( component ) ); break;
	case Pine::EComponentType::Light:
		RenderLight( dynamic_cast< Pine::Light* >( component ) ); break;
	case Pine::EComponentType::Behavior:
		RenderBehavior( dynamic_cast< Pine::Behavior* >( component ) ); break;
	case Pine::EComponentType::TerrainRenderer:
		RenderTerrainRenderer( dynamic_cast< Pine::TerrainRenderer* >( component ) ); break;
	default:
		break;
	}

}
