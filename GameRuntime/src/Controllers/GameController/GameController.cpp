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

void GameController::OnSetup( )
{
	Pine::Log->Message( "GameController::OnSetup( )" );
}

void GameController::OnRender( )
{
	
}

void GameController::OnUpdate( float deltaTime )
{
}
