#include "Mesh.hpp"

Pine::Mesh::Mesh() {
	m_VertexArray = std::make_unique<VertexArray>();
	m_VertexArray->Create();

	m_Material = Assets::GetAsset<Pine::Material>("Engine\\Materials\\Default.mat");
}

Pine::Material* Pine::Mesh::GetMaterial() const {
	return m_Material;
}

void Pine::Mesh::SetMaterial(Material* material) {
	m_Material = material;
}

Pine::VertexArray* Pine::Mesh::GetVertexArray() const {
	return m_VertexArray.get();
}

uint64_t Pine::Mesh::GetRenderCount() const {
	return m_RenderCount;
}

bool Pine::Mesh::HasElementBuffer() const {
	return m_HasElementBuffer;
}

void Pine::Mesh::SetVertices(const std::vector<float>& vertices) {
	m_VertexArray->Bind();
	m_VertexArray->StoreFloatBuffer(vertices, 3, 0);
	m_RenderCount = vertices.size();
}

void Pine::Mesh::SetIndices(const std::vector<int>& indices) {
	m_VertexArray->Bind();
	m_VertexArray->StoreIntBuffer(indices);
	m_RenderCount = indices.size();
	m_HasElementBuffer = true;
}

void Pine::Mesh::SetNormals(const std::vector<float>& normals) {
	m_VertexArray->Bind();
	m_VertexArray->StoreFloatBuffer(normals, 3, 1);
}

void Pine::Mesh::SetUvs(const std::vector<float>& uvs) {
	m_VertexArray->Bind();
	m_VertexArray->StoreFloatBuffer(uvs, 2, 2);
}

void Pine::Mesh::Dispose() {
	m_VertexArray->Dispose();
}
