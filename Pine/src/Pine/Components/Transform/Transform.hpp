#pragma once
#include "../IComponent/IComponent.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Pine
{

	class Transform : public IComponent
	{
	private:
		glm::mat4 m_TransformationMatrix;

        glm::vec3 m_EulerAngles;

        glm::vec3 m_LastPosition;
        glm::quat m_LastRotation;
        glm::vec3 m_LastScale;

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

        // Calculate the final position, taking account of every parent's position/rotation.
        glm::vec3 GetPositionSum( ) const;
        glm::quat GetRotationSum( ) const;

        void SetEulerAngles(glm::vec3 eulerAngles);
        void Rotate(const glm::vec3& eulerAngles);

        glm::vec3 Position = glm::vec3(0.f);
        glm::vec3 Scale = glm::vec3(1.f);
        glm::quat Rotation;

        // Read only, to apply euler angles, use either SetEulerAngles() or calculate a new quaternion directly via glm.
        glm::vec3 EulerAngles;
    };

}