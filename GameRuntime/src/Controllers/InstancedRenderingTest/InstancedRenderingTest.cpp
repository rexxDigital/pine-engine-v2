#include "InstancedRenderingTest.hpp"

#include <glfw/glfw3.h>

#include "Pine/Assets/Assets.hpp"
#include "Pine/Components/Light/Light.hpp"
#include "Pine/Entitylist/EntityList.hpp"
#include <Pine/Assets/Model/Model.hpp>

#include "Pine/Components/ModelRenderer/ModelRenderer.hpp"
#include "Pine/Core/Log/Log.hpp"

void InstancedRenderingTest::OnSetup( )
{
	auto cube = Pine::Assets->GetAsset<Pine::Model>( "Assets\\Engine\\Primitive Shapes\\cube.fbx" );

	Pine::Log->Message( "yep" );

	for ( int i = 0; i < 30; i++ )
	{
		auto e = Pine::EntityList->CreateEntity( "geoff #" + std::to_string( i ) );

		e->AddComponent( Pine::ComponentType::ModelRenderer );
		e->GetComponent<Pine::ModelRenderer>( )->SetModel( cube );

		e->GetTransform( )->Position.x = i * 10.f;
	}
}

void InstancedRenderingTest::OnUpdate( const float deltaTime )
{

}