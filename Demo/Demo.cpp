#include <iostream>
#include <vector>

#include <Pine/Pine.hpp>
#include <Pine/Entity/Entity.hpp>
#include <Pine/Entitylist/Entitylist.hpp>
#include <Pine/Components/ModelRenderer/ModelRenderer.hpp>
#include <Pine/Components/Camera/Camera.hpp>
#include <Pine/Components/Light/Light.hpp>
#include <Pine/Rendering/RenderManager/RenderManager.hpp>
#include <Pine/Rendering/Skybox/Skybox.hpp>
#include <Pine/Assets/Texture3D/Texture3D.hpp>

Pine::Entity* camera = nullptr;

void SetupSampleScene( ) {
//	auto planet = Pine::EntityList::CreateEntity( );

//	planet->AddComponent( new Pine::ModelRenderer( ) );

	auto model = Pine::Assets::GetAsset<Pine::Model>( "Assets\\Engine\\untitled.obj" );
	auto mesh = model->GetMeshList( )[ 0 ];

	mesh->GetMaterial( )->AmbientColor( ) = glm::vec3( 0.3f, 0.3f, 0.3f );

	mesh->GetMaterial( )->SpecularColor( ) = glm::vec3( 0.f, 0.f, 0.f );
	mesh->GetMaterial( )->DiffuseColor( ) = glm::vec3( 0.5f, 0.5f, 0.5f );
	mesh->GetMaterial( )->SetShininiess( 16.f );

	mesh->GetMaterial( )->SetDiffuse( Pine::Assets::GetAsset<Pine::Texture2D>( "Assets\\diffuse-front.png" ) );

	//planet->GetComponent<Pine::ModelRenderer>( )->SetTargetModel( model );
	//planet->GetTransform( )->Position.z = 15.f;
	//planet->GetTransform( )->Position.y = -2.f;

	camera = Pine::EntityList::CreateEntity( "Camera" );

	camera->AddComponent( new Pine::Camera( ) );
	camera->GetTransform( )->Position.z = -2.f;

	auto light = Pine::EntityList::CreateEntity( "light" );

	light->AddComponent( new Pine::Light( ) );
	light->GetTransform( )->Position = glm::vec3( 0.f, 0.f, -20.f );

	Pine::RenderManager::GetRenderingContext( )->m_Camera = camera->GetComponent<Pine::Camera>( );

	Pine::Skybox::SetSkyboxCubemap( Pine::Assets::GetAsset<Pine::Texture3D>( "Assets\\Engine\\Skyboxes\\Space.cmap" ) );
}

void OnRender( ) {
	camera->GetTransform( )->Rotation.y -= 0.01f;
}

int main( ) {
	if ( !Pine::Setup( ) ) {
		return 1;
	}

	Pine::Assets::LoadFromDirectory( "Assets" );

	SetupSampleScene( );

	Pine::RenderManager::SetRenderingCallback( OnRender );

	Pine::Run( );

	Pine::Terminate( );

	return 0;
}
