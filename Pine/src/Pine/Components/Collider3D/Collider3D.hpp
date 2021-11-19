#pragma once
#include <glm/vec3.hpp>

#include "../IComponent/IComponent.hpp"
#include <reactphysics3d/reactphysics3d.h>

namespace Pine
{

	enum class ColliderType
	{
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
		ColliderType m_Type = ColliderType::Box;

		// A pointer to the underlying shape, t.ex BoxCollider
		reactphysics3d::CollisionShape* m_Shape = nullptr;

		// Used for all shapes
		glm::vec3 m_Position = glm::vec3( );
		glm::vec3 m_Size = glm::vec3( 1.f, 1.f, 1.f );

		reactphysics3d::Transform m_CollisionBodyTransform;
		reactphysics3d::Transform m_CollisionTransform;

		reactphysics3d::Collider* m_Collider = nullptr;
		reactphysics3d::CollisionBody* m_CollisionBody = nullptr;

		bool m_ShapeUpdated = false;

		void UpdateBody( );

		void CreateShape( );
		void DisposeShape( );
		void UpdateShape( );
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

		// Used for capsule only
		void SetHeight( float height );
		float GetHeight( ) const;

		bool PollShapeUpdated( );

		reactphysics3d::CollisionShape* GetCollisionShape( ) const;
		reactphysics3d::Collider* GetCollider( ) const; // WARNING: This object will only be present if no RigidBody(s) are attached to the parent.
		 
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
