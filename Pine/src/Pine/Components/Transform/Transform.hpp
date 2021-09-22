#pragma once
#include "../IComponent/IComponent.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace Pine
{

	class Transform : public IComponent
	{
	private:
		glm::mat4 m_TransformationMatrix;

		float m_LastPositionLen = 0.0f;
		float m_LastRotationLen = 0.0f;
		float m_LastScaleLen = 0.0f;

		glm::vec3 m_Forward;
		glm::vec3 m_Right;
		glm::vec3 m_Up;

		void BuildDirections( );
		void BuildTransformationMatrix( );
	public:
		Transform( );

		void OnSetup( ) override;
		void OnUpdate( float deltaTime ) override;
		void OnRender( ) override;

		void SaveToJson( nlohmann::json& j ) override;
		void LoadFromJson( nlohmann::json& j ) override;

		glm::mat4& GetTransformationMatrix( );

		const glm::vec3& GetForward( ) const;
		const glm::vec3& GetRight( ) const;
		const glm::vec3& GetUp( ) const;

		glm::vec3 Position = glm::vec3( 0.f, 0.f, 0.f );
		glm::vec3 Rotation = glm::vec3( 0.f, 0.f, 0.f );
		glm::vec3 Scale = glm::vec3( 1.f, 1.f, 1.f );
	};

}