#include "VertexArray.hpp"

#include <gl/glew.h>

uint32_t Pine::VertexArray::CreateBuffer()
{
	uint32_t id;

	glGenBuffers(1, &id);
	m_Buffers.push_back(id);

	return id;
}

void Pine::VertexArray::Create()
{
	glGenVertexArrays(1, &m_Id);
}

void Pine::VertexArray::Bind() const
{
	glBindVertexArray(m_Id);
}

void Pine::VertexArray::Dispose()
{
	glDeleteBuffers(static_cast<GLsizei>(m_Buffers.size()), m_Buffers.data());
	glDeleteVertexArrays(1, &m_Id);
}

void Pine::VertexArray::StoreFloatBuffer(const std::vector<float> floatBuffer, const int vectorSize, const int binding)
{
	const auto buffer = CreateBuffer();

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, floatBuffer.size() * sizeof(float), floatBuffer.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(binding, vectorSize, GL_FLOAT, false, 0, nullptr);
	glEnableVertexAttribArray(binding);
}

void Pine::VertexArray::StoreIntBuffer(const std::vector<int> intBuffer)
{
	const auto buffer = CreateBuffer();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, intBuffer.size() * sizeof(int), intBuffer.data(), GL_STATIC_DRAW);
}

uint32_t Pine::VertexArray::GetId() const
{
	return m_Id;
}
