#pragma once
#include "Pine/Components/Camera/Camera.hpp"
#include "Pine/Entity/Entity.hpp"

namespace Editor::EditorEntity
{

	void Create( );

	Pine::Entity* GetEntity( );

	Pine::Camera* GetCamera( );

}
