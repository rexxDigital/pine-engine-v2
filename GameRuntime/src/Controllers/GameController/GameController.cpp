#include "GameController.hpp"

#include <glm/ext/matrix_projection.hpp>

#include "Pine/Pine.hpp"
#include "Pine/Components/Collider3D/Collider3D.hpp"
#include "Pine/Components/ModelRenderer/ModelRenderer.hpp"
#include "Pine/Core/Math/Math.hpp"
#include "Pine/Entity/Entity.hpp"
#include "Pine/Entitylist/EntityList.hpp"
#include "Pine/Input/Input.hpp"
#include "Pine/Rendering/DebugOverlay/DebugOverlay.hpp"
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
	ent->AddComponent( Pine::ComponentType::ModelRenderer );

	ent->GetComponent<Pine::ModelRenderer>( )->SetModel( Pine::Assets->GetAsset<Pine::Model>( "Assets\\Engine\\Primitive Shapes\\cube.fbx" ) );
	ent->GetComponent<Pine::ModelRenderer>( )->SetActive( false ); // We'll only render it while it's hovered

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
	const auto mouse = Pine::Input->GetMousePosition( );

	float closestChunk = std::numeric_limits<float>::max( );
	int closestChunkIndex = 0;

	for ( int i = 0; i < m_Chunks.size( ); i++ )
	{
		// Compute 3d coords to 2d screen coords, this will probably get wrapped cleaner into the engine in the future.
		const auto screenPos = Pine::Math->WorldToScreen( m_Chunks[ i ].m_Entity->GetTransform( )->Position, cam );

		// Calculate the delta between our cursor and the screen coords.
		auto delta = glm::vec3( mouse, 0 ) - screenPos;

		const float deltaLen = glm::length( delta );

		if ( closestChunk > deltaLen )
		{
			closestChunk = deltaLen;
			closestChunkIndex = i;
		}

		m_Chunks[ i ].m_Entity->GetComponent<Pine::ModelRenderer>( )->SetActive( false );
	}

	m_Chunks[ closestChunkIndex ].m_Entity->GetComponent<Pine::ModelRenderer>( )->SetActive( true );
}

void GameController::OnUpdate( float deltaTime )
{
}
