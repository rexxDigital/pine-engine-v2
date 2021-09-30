#pragma once
#include <glm/vec3.hpp>

#include "../IComponent/IComponent.hpp"
#include <reactphysics3d/reactphysics3d.h>

namespace Pine
{

	enum class ColliderType
	{
		Invalid,
		Box,
		Sphere,
		Capsule,
		ConvexMesh,
		ConcaveMesh,
		Heightfield
	};

	class Collider3D : public IComponent
	{
	private:
		ColliderType m_Type = ColliderType::Invalid;

		reactphysics3d::CollisionBody* m_Body = nullptr;

		// A pointer to the underlying shape, t.ex BoxCollider
		reactphysics3d::CollisionShape* m_Shape = nullptr;

		// Used for all shapes
		glm::vec3 m_Position = glm::vec3( );
		glm::vec3 m_Size = glm::vec3( 1.f, 1.f, 1.f );

		reactphysics3d::Transform m_PhysTransform;

		void UpdateBody( );

		void CreateShape( );
		void DisposeShape( );
		void UpdateShape( ) const;
	public:
		Collider3D( );

		void SetColliderType( ColliderType type );
		ColliderType GetColliderType( ) const;

		void SetPosition( glm::vec3 position );
		const glm::vec3& GetPosition( ) const;

		void SetSize( glm::vec3 size );
		const glm::vec3& GetSize( ) const;

		// Used for sphere and capsule
		void SetRadius( float radius );
		float GetRadius( ) const;

		// Used for capsule
		void SetHeight( float height );
		float GetHeight( ) const;

		void OnSetup( ) override;
		void OnUpdate( float deltaTime ) override;

		void OnCreated( ) override;
		void OnCopied( const IComponent* old ) override;
		void OnDestroyed( ) override;

		void OnPrePhysicsUpdate( );

		void SaveToJson( nlohmann::json& j ) override;
		void LoadFromJson( nlohmann::json& j ) override;
	};


}
