#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../Interfaces/Interfaces.hpp"

namespace Pine
{

	class Camera;

	class IMath : public IInterface
	{
	private:
	public:

		// Transforms 3D coordinates into 2D coordinates on the screen,
		// use the depth (z value) to determine if it's visible on the screen right now.
		virtual glm::vec3 WorldToScreen( const glm::vec3& inputPosition ) = 0;
		virtual glm::vec3 WorldToScreen( const glm::vec3& inputPosition, Camera* camera ) = 0;

	};

}