#include "Collider.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../Entity/Entity.hpp"
#include "../../PhysicsManager/PhysicsManager.hpp"

Pine::Collider::Collider( )
{
	m_ComponentType = EComponentType::Collider;
}

void Pine::Collider::SetColliderType( ColliderType type )
{
	if ( m_Shape )
	{

	}

	m_Type = type;
}

Pine::ColliderType Pine::Collider::GetColliderType( ) const
{
	return m_Type;
}

void Pine::Collider::OnCreated( )
{
}

void Pine::Collider::OnDestroyed( )
{
}

void Pine::Collider::OnSetup( )
{

}

void Pine::Collider::OnUpdate( float deltaTime )
{

}

void Pine::Collider::SaveToJson( nlohmann::json& j )
{
}

void Pine::Collider::LoadFromJson( nlohmann::json& j )
{

}
