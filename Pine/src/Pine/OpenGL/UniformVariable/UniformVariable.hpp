#pragma once

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace Pine
{

	class ShaderProgram;

	class UniformVariable
	{
	private:
		int m_Location = 0;
		bool m_Valid = false;
	public:
		UniformVariable( const ShaderProgram* program, const std::string& name );

		int GetLocation( ) const;
		bool IsValid( ) const;

		void LoadInteger( int value );
		void LoadFloat( float value );

		void LoadVector2( const glm::vec2& value );
		void LoadVector3( const glm::vec3& value );
		void LoadVector4( const glm::vec4& value );

        void LoadVector2( const glm::ivec2& value );
        void LoadVector3( const glm::ivec3& value );
        void LoadVector4( const glm::ivec4& value );

		void LoadMatrix4( const glm::mat4& value );
	};


}