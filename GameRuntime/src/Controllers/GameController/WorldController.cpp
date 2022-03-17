#include "WorldController.hpp"

#include <glm/ext/matrix_projection.hpp>
#include <random>

#include "Pine/Pine.hpp"
#include "Pine/Components/Collider3D/Collider3D.hpp"
#include "Pine/Components/ModelRenderer/ModelRenderer.hpp"
#include "Pine/Core/Math/Math.hpp"
#include "Pine/Entity/Entity.hpp"
#include "Pine/Entitylist/EntityList.hpp"
#include "Pine/Input/Input.hpp"
#include "Pine/Rendering/DebugOverlay/DebugOverlay.hpp"
#include "Pine/Rendering/RenderManager/RenderManager.hpp"
#include "Pine/Assets/Level/Level.hpp"

void WorldController::OnSetup( )
{
    Pine::Log->Message( "WorldController::OnSetup( )" );

    auto treeBlueprint = Pine::Assets->GetAsset<Pine::Blueprint>( "Projects\\Debug\\Blueprints\\Tree.bpt" );
    if ( treeBlueprint )
    {
        constexpr int treeAmount = 50;

        srand( time( NULL ) );

        for ( int i = 0; i < treeAmount; i++ )
        {
            auto tree = treeBlueprint->SpawnEntity( );

            tree->GetTransform( )->Position = glm::vec3( (rand( ) % 200 + 1) - 100, 0.f, (rand( ) % 200 + 1) - 100 );
        }
    } else
    {
        Pine::Log->Error( "Failed to find tree blueprint." );
    }
}

void WorldController::OnRender( )
{

}

void WorldController::OnUpdate( float deltaTime )
{

}