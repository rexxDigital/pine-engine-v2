#include "GameController.hpp"

#include <glm/ext/matrix_projection.hpp>
#include <Pine/Pine.hpp>

#include "Pine/Components/Collider3D/Collider3D.hpp"
#include "Pine/Components/ModelRenderer/ModelRenderer.hpp"
#include "Pine/Entity/Entity.hpp"
#include "Pine/Entitylist/EntityList.hpp"
#include "Pine/Input/Input.hpp"
#include "Pine/Rendering/RenderManager/RenderManager.hpp"

constexpr float ChunkSize = 10.f;

void GameController::CreateChunk( int x, int y )
{
	m_Chunks.emplace_back( );

	auto& chunk = m_Chunks[ m_Chunks.size( ) - 1 ];

	const auto ent = Pine::EntityList->CreateEntity( );

	// Set name to Chunk(x, y)
	ent->SetName( "Chunk (" + std::to_string( x ) + ", " + std::to_string( y ) + ")" );

	// Setup position correctly in a grid pattern
	ent->GetTransform( )->Position = m_ChunkSpawnLocation->GetTransform( )->Position - glm::vec3( ChunkSize * y, 0.f, ChunkSize * x );

	// Create the chunk hover model
	ent->AddComponent( Pine::EComponentType::ModelRenderer );

	ent->GetComponent<Pine::ModelRenderer>( )->SetTargetModel( Pine::Assets->GetAsset<Pine::Model>( "Assets\\Engine\\Primitive Shapes\\cube.fbx" ) );
	ent->GetComponent<Pine::ModelRenderer>( )->SetActive( false ); // We'll only render it while it's 

	// Create the collision box
	/*ent->AddComponent( Pine::EComponentType::Collider3D );

	ent->GetComponent<Pine::Collider3D>( )->SetColliderType( Pine::ColliderType::Box );
	ent->GetComponent<Pine::Collider3D>( )->SetSize( glm::vec3( 5.f, 0.1f, 5.f ) );*/
	//ent->GetComponent<Pine::Collider3D>( )->GetCollider( )->setUserData( &chunk );

	// 5 seems to work fine for the current chunk size.
	ent->GetTransform( )->Scale = glm::vec3( 5.f, 0.1f, 5.f );

	chunk.m_Entity = ent;
	chunk.m_Position = glm::vec2( x, y );
}

void GameController::OnSetup( )
{
	Pine::Log->Message( "GameController::OnSetup( )" );

	// We could get these from our children but this will do.
	m_EnemySpawnLocation = Pine::EntityList->FindEntity( "Enemy Spawn Location" );
	m_ChunkSpawnLocation = Pine::EntityList->FindEntity( "Tower Chunk Spawn" );

	// Prepare chunks
	for ( int y = 0; y < 12; y++ )
	{
		for ( int x = 0; x < 12; x++ )
		{
			CreateChunk( x, y );
		}
	}
}

void GameController::OnRender( )
{
	// Compute screen coordinates for each chunk.
	// Due to the camera constantly being on a top down view of the world,
	// we can check if the mouse cursor delta is small with the center of a chunk,
	// meaning that we are currently hovering over it.

	const auto cam = Pine::RenderManager->GetRenderingContext( )->m_Camera;

	const auto mouseX = Pine::Input->GetMouseX( );
	const auto mouseY = Pine::Input->GetMouseY( );

	for ( int y = 0; y < 12; y++ )
	{
		for ( int x = 0; x < 12; x++ )
		{
			auto screenPos = glm::project( m_Chunks[ y + x ].m_Entity->GetTransform( )->Position, cam->GetViewMatrix( ) * m_Chunks[ y + x ].m_Entity->GetTransform( )->GetTransformationMatrix( ), cam->GetProjectionMatrix( ), glm::vec4( 0.f, 0.f, 600.f, 600.f ) );
			auto delta = glm::vec3( mouseX, mouseY, 0.f ) - screenPos;

			// TODO: Implement a debug overlay to quickly render debug stuff.

			if ( length( delta ) < 50.f )
			{
				m_Chunks[ y + x ].m_Entity->GetComponent<Pine::ModelRenderer>( )->SetActive( true );
			}
		}
	}

}

void GameController::OnUpdate( float deltaTime )
{

}
