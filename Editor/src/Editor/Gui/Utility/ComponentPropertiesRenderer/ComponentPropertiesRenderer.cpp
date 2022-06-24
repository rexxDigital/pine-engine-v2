#include "ComponentPropertiesRenderer.hpp"
#include "../../Widgets/Widgets.hpp"

#include <Pine/Components/Transform/Transform.hpp>
#include <Pine/Components/ModelRenderer/ModelRenderer.hpp>
#include <Pine/Components/Camera/Camera.hpp>
#include <Pine/Rendering/RenderManager/RenderManager.hpp>

#include "Editor/ProjectManager/ProjectManager.hpp"
#include "ImGui/imgui.h"
#include "Pine/Components/Light/Light.hpp"
#include "Pine/Components/RigidBody//RigidBody.hpp"
#include "Pine/Components/TerrainRenderer/TerrainRenderer.hpp"
#include "Pine/Components/Collider3D/Collider3D.hpp"
#include "Pine/Components/NativeScript/NativeScript.hpp"

using namespace Editor::Gui;

void UpdateAssetCache( );

namespace
{

	void RenderTransform( Pine::Transform* transform )
	{
		if ( !transform )
			return;

		Widgets::Vector3( "Position", transform->Position );
		Widgets::Vector3( "Rotation", transform->Rotation );
		Widgets::Vector3( "Scale", transform->Scale );
	}

	void RenderModelRenderer( Pine::ModelRenderer* renderer )
	{
		if ( !renderer )
			return;

		const auto modelRet = Widgets::AssetPicker( "Model", renderer->GetModel( ), true, Pine::AssetType::Model );
		if ( modelRet.valid )
		{
			renderer->SetModel( reinterpret_cast< Pine::Model* >( modelRet.asset ) );
		}

		const auto materialRet = Widgets::AssetPicker( "Override model material", renderer->GetMaterialOverride( ), true, Pine::AssetType::Material );
		if ( materialRet.valid )
		{
			renderer->SetMaterialOverride( reinterpret_cast< Pine::Material* >( materialRet.asset ) );
		}
	}

	void RenderCamera( Pine::Camera* camera )
	{
		float nearPlane = camera->GetNearPlane( );
		float farPlane = camera->GetFarPlane( );
		float fieldOfView = camera->GetFieldOfView( );
		bool isActiveCamera = Pine::RenderManager->GetRenderingContext( )->m_Camera == camera;

		if ( Widgets::Checkbox( "Active", isActiveCamera ) )
		{
			Pine::RenderManager->GetRenderingContext( )->m_Camera = isActiveCamera ? camera : nullptr;
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
		if ( Widgets::Combobox( "Type", lightType, "Directional\0Point light\0Spot light" ) )
		{
			light->SetLightType( static_cast< Pine::LightType >( lightType ) );
		}

		auto lightColor = light->GetLightColor( );
		if ( Widgets::ColorPicker( "Light Color", lightColor ) )
		{
			light->SetLightColor( lightColor );
		}

		if ( light->GetLightType(  ) != Pine::LightType::Directional )
		{
			auto attenuation = light->GetAttenuation( );

			if ( Widgets::SliderFloat( "Linear", attenuation.y, 0.0001f, 1.f, "%.5f" ) )
			{
				light->SetAttenuation( attenuation );
			}

			if ( Widgets::SliderFloat( "Quadratic", attenuation.z, 0.0001f, 2.f, "%.5f" ) )
			{
				light->SetAttenuation( attenuation );
			}

			if ( light->GetLightType(  ) == Pine::LightType::SpotLight )
			{
				float spotlightAngle = light->GetSpotlightAngle( );
				float spotlightSmoothness = light->GetSpotlightSmoothness( );

				if ( Widgets::SliderFloat( "Spotlight angle", spotlightAngle, 0.f, 180.f, "%.1f deg" ) )
				{
					light->SetSpotlightAngle( spotlightAngle );
				}

				if ( Widgets::SliderFloat( "Spotlight smoothness", spotlightSmoothness, 0.f, 180.f, "%.1f deg" ) )
				{
					light->SetSpotlightSmoothness( spotlightSmoothness );
				}
			}
		}
	}

	void RenderTerrainRenderer( Pine::TerrainRenderer* terrainRenderer )
	{
		if ( !terrainRenderer )
			return;

		const auto terrain = terrainRenderer->GetTerrain( );

		const auto terrainRet = Widgets::AssetPicker( "Terrain Object", terrain, true, Pine::AssetType::Terrain );
		if ( terrainRet.valid )
		{
			terrainRenderer->SetTerrain( dynamic_cast< Pine::Terrain* >( terrainRet.asset ) );
		}

		ImGui::Spacing( );

		if ( terrain == nullptr )
		{
			ImGui::Text( "No terrain object attached." );

			if ( ImGui::Button( "Create new Terrain", ImVec2( 200.f, 30.f ) ) )
			{
				const std::string terrainPath = Editor::ProjectManager::GetCurrentProjectDirectory( ) + "/terrain.ter";

				auto terrain = new Pine::Terrain;

				terrain->SetFilePath( terrainPath );
				terrain->SaveToFile( );

				delete terrain;

				terrainRenderer->SetTerrain( dynamic_cast< Pine::Terrain* >( Pine::Assets->LoadFromFile( terrainPath ) ) );

				UpdateAssetCache( );
			}

			return;
		}

		const auto& chunks = terrain->GetChunks( );

		ImGui::Text( "Chunks: %d", chunks.size( ) );

		if ( ImGui::Button( "Create Chunk" ) )
		{
			terrain->CreateChunk( glm::ivec2( 0, 0 ) );
		}
	}

	void RenderRigidBody( Pine::RigidBody* rigidBody )
	{
		int currentType = static_cast< int >( rigidBody->GetRigidBodyType( ) );
		float currentMass = rigidBody->GetMass( );
		bool currentGravityEnabled = rigidBody->GetGravityEnabled( );

		if ( Widgets::Combobox( "Type", currentType, "Static\0Kinematic\0Dynamic\0" ) )
		{
			rigidBody->SetRigidBodyType( static_cast< Pine::RigidBodyType >( currentType ) );
		}

		if ( Widgets::SliderFloat( "Mass", currentMass, 0.f, 10000.f, "%.2f kg" ) )
		{
			rigidBody->SetMass( currentMass );
		}

		if ( Widgets::Checkbox( "Enable Gravity", currentGravityEnabled ) )
		{
			rigidBody->SetGravityEnabled( currentGravityEnabled );
		}
	}

	void RenderCollider3D( Pine::Collider3D* collider3d )
	{
		int currentType = static_cast< int >( collider3d->GetColliderType( ) );

		glm::vec3 currentPosition = collider3d->GetPosition( );
		glm::vec3 currentSize = collider3d->GetSize( );

		float currentRadius = collider3d->GetRadius( );
		float currentHeight = collider3d->GetHeight( );

		if ( Widgets::Combobox( "Collider Type", currentType, "Box\0Sphere\0Capsule\0Convex Mesh\0Concave Mesh\0Height field\0" ) )
		{
			collider3d->SetColliderType( static_cast< Pine::ColliderType >( currentType ) );
		}

		ImGui::Spacing( );

		if ( Widgets::Vector3( "Collider Position", currentPosition ) )
		{
			collider3d->SetPosition( currentPosition );
		}

		if ( currentType == 0 )
		{
			if ( Widgets::Vector3( "Collider Size", currentSize ) )
			{
				collider3d->SetSize( currentSize );
			}
		}

		if ( currentType == 1 )
		{
			if ( Widgets::SliderFloat( "Radius", currentRadius, 0.f, 10.f, "%.1f" ) )
			{
				collider3d->SetRadius( currentRadius );
			}
		}

		if ( currentType == 2 )
		{
			if ( Widgets::SliderFloat( "Radius", currentRadius, 0.f, 10.f, "%.1f m" ) )
			{
				collider3d->SetRadius( currentRadius );
			}

			if ( Widgets::SliderFloat( "Height", currentHeight, 0.f, 10.f, "%.1f m" ) )
			{
				collider3d->SetHeight( currentHeight );
			}
		}

	}

	void RenderNativeScript( Pine::NativeScript* nativeScript )
	{
		static bool showNativeScriptSelector = false;

		const auto renderNativeScriptSelector = [ ]
		{

		};

		ImGui::Columns( 2, nullptr, false );

		ImGui::Text( "Script" );

		ImGui::NextColumn( );

		char buff[ 64 ];

		strcpy( buff, nativeScript->GetFactoryName( ).c_str( ) );

		ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2( 3.f, 4.f ) );
		ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail( ).x - 60.f );

		if ( ImGui::InputText( "##NativeScriptName", buff, 64, 0, 0 ) )
		{
			nativeScript->SetFactoryName( buff );
		}

		ImGui::SameLine( );

		if ( ImGui::Button( "...##NativeScript" ) )
		{
			showNativeScriptSelector = true;
		}

		ImGui::PopStyleVar( );

		ImGui::Columns( 1 );

		if ( showNativeScriptSelector )
		{
			renderNativeScriptSelector( );
		}


	}

}

void Utility::ComponentPropertiesRenderer::RenderComponentProperties( Pine::IComponent* component )
{

	switch ( component->GetType( ) )
	{
	case Pine::ComponentType::Transform:
		RenderTransform( dynamic_cast< Pine::Transform* >( component ) ); break;
	case Pine::ComponentType::ModelRenderer:
		RenderModelRenderer( dynamic_cast< Pine::ModelRenderer* >( component ) ); break;
	case Pine::ComponentType::Camera:
		RenderCamera( dynamic_cast< Pine::Camera* >( component ) ); break;
	case Pine::ComponentType::Light:
		RenderLight( dynamic_cast< Pine::Light* >( component ) ); break;
	case Pine::ComponentType::TerrainRenderer:
		RenderTerrainRenderer( dynamic_cast< Pine::TerrainRenderer* >( component ) ); break;
	case Pine::ComponentType::RigidBody:
		RenderRigidBody( dynamic_cast< Pine::RigidBody* >( component ) ); break;
	case Pine::ComponentType::Collider3D:
		RenderCollider3D( dynamic_cast< Pine::Collider3D* >( component ) ); break;
	case Pine::ComponentType::NativeScript:
		RenderNativeScript( dynamic_cast< Pine::NativeScript* >( component ) ); break;
	default:
		break;
	}



}
