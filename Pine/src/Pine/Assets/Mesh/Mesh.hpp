#pragma once
#include "../IAsset/IAsset.hpp"
#include "../Material/Material.hpp"
#include "../../OpenGL/VertexArray/VertexArray.hpp"

namespace Pine
{

	namespace Internal
	{
		// This is how it's accessed in the shaders, and shall NOT be changed.
		// Standards for the engine.
		constexpr int VERTEX_ARRAY_BUFFER = 0;
		constexpr int NORMAL_ARRAY_BUFFER = 1;
		constexpr int UV_ARRAY_BUFFER = 2;
	}

	class Model;

	class Mesh
	{
	private:
		AssetContainer<Material*> m_Material;
		Model* m_ParentModel = nullptr;

		std::unique_ptr<VertexArray> m_VertexArray = nullptr;

		uint64_t m_RenderCount = 0;
		bool m_HasElementBuffer = false;

		glm::vec3 m_Mins;
		glm::vec3 m_Maxs;
	public:
		Mesh( Model* model );

		Model* GetParentModel( ) const;

		VertexArray* GetVertexArray( ) const;

		Material* GetMaterial( ) const;
		void SetMaterial( Material* material );

		uint64_t GetRenderCount( ) const;
		bool HasElementBuffer( ) const;

		const glm::vec3& GetMins( ) const;
		const glm::vec3& GetMaxs( ) const;

		void SetMaxs( glm::vec3 aabb );
		void SetMins( glm::vec3 aabb );

		void SetVertices( const std::vector<float>& vertices );
		void SetIndices( const std::vector<int>& indices );
		void SetNormals( const std::vector<float>& normals );
		void SetUvs( const std::vector<float>& uvs );

		void Dispose( );
	};

}