#pragma once
#include "../IAsset/IAsset.hpp"
#include "../Material/Material.hpp"
#include "../../OpenGL/VertexArray/VertexArray.hpp"

namespace Pine {

	class Mesh {
	private:
		Material* m_Material = nullptr;

		std::unique_ptr<VertexArray> m_VertexArray = nullptr;
	
		uint64_t m_RenderCount = 0;
		bool m_HasElementBuffer = false;
	public:
		Mesh();

		VertexArray* GetVertexArray() const;

		Material* GetMaterial() const;
		void SetMaterial(Material* material);

		uint64_t GetRenderCount() const;
		bool HasElementBuffer() const;

		void SetVertices(const std::vector<float>& vertices);
		void SetIndices(const std::vector<int>& indices);
		void SetNormals(const std::vector<float>& normals);
		void SetUvs(const std::vector<float>& uvs);

		void Dispose();
	};

}