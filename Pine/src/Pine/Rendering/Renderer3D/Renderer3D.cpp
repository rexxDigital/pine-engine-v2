#include "Renderer3D.hpp"
#include <GL/glew.h>
#include "../../UniformBuffers/UniformBuffers.hpp"

namespace {
	// The mesh we're supposed to render.
	Pine::Mesh* g_CurrentRenderMesh = nullptr;
	
	// Shader stuff.
	Pine::Shader* g_CurrentShader = nullptr;
	Pine::UniformVariable* g_ShaderTransformationVariable = nullptr;

	// The texture we use if there is no texture applied, 1x1 white.
	Pine::Texture2D* g_DefaultTexture = nullptr;

	// Some optimizations for OpenGL's current texture.
	int g_CurrentBoundTexture[32] = { };
}

void Pine::Renderer3D::RenderVertexArray(const VertexArray* vao, int renderCount, bool indices)
{
	vao->Bind();

	if (indices)
	{
		glDrawElements(GL_TRIANGLES, renderCount, GL_UNSIGNED_INT, nullptr);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, renderCount);
	}
}

void Pine::Renderer3D::PrepareMesh(Pine::Mesh* mesh) {
	auto material = mesh->GetMaterial();
	
	mesh->GetVertexArray()->Bind();

	if (material->GetShader() != g_CurrentShader) {
		SetShader(material->GetShader());
	}
	 
	// Diffuse texture
	Texture2D* diffuseTexture = g_DefaultTexture;
	if (material->GetDiffuse() != nullptr) {
		diffuseTexture = material->GetDiffuse();
	}

	// Only bind the texture if required.
	if (g_CurrentBoundTexture[0] != diffuseTexture->GetId()) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture->GetId());

		g_CurrentBoundTexture[0] = diffuseTexture->GetId();
	}

	// Specular map texture
	Texture2D* specularMapTexture = g_DefaultTexture;
	if (material->GetSpecular() != nullptr) {
		specularMapTexture = material->GetSpecular();
	}

	g_CurrentShader->GetUniformVariable("materialSamplers.specular")->LoadInteger(1);

	// Only bind the texture if required.
	if (g_CurrentBoundTexture[1] != specularMapTexture->GetId()) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMapTexture->GetId());

		g_CurrentBoundTexture[1] = specularMapTexture->GetId();
	}

	// Apply material data
	auto materialDataBuffer = UniformBuffers::GetMaterialBufferData();
	
	materialDataBuffer->diffuseColor = material->DiffuseColor();
	materialDataBuffer->specularColor = material->SpecularColor();
	materialDataBuffer->ambientColor = material->AmbientColor();
	materialDataBuffer->shininiess = material->GetShininiess();

	// Due to the amazing design of this game engine, the material uniform buffer SHOULD already be bound by now,
	// if not, I've fucked something up.
	UniformBuffers::GetMaterialUniformBuffer()->UploadData(0, sizeof(UniformBuffers::MaterialBufferData_t), materialDataBuffer);

	g_CurrentRenderMesh = mesh;
	g_CurrentShader->Use();
}

void Pine::Renderer3D::RenderMesh(const glm::mat4& transformationMatrix) {
	if (g_ShaderTransformationVariable != nullptr) {
		g_ShaderTransformationVariable->LoadMatrix4(transformationMatrix);
	}

	if (g_CurrentRenderMesh->HasElementBuffer()) {
		glDrawElements(GL_TRIANGLES, g_CurrentRenderMesh->GetRenderCount(), GL_UNSIGNED_INT, nullptr);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, g_CurrentRenderMesh->GetRenderCount());
	}
}

Pine::Shader* Pine::Renderer3D::GetShader() {
	return g_CurrentShader;
}

void Pine::Renderer3D::SetShader(Pine::Shader* shader) {
	g_CurrentShader = shader;
	
	// Set cached uniform variables
	g_ShaderTransformationVariable = shader->GetUniformVariable("transform");
}

void Pine::Renderer3D::Setup() {
	g_DefaultTexture = Assets::GetAsset<Texture2D>("Engine\\DefaultTexture.png");
}

void Pine::Renderer3D::Dispose() {

}

