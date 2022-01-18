#include "Renderer2D.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <GL/glew.h>
#include <glm/ext/matrix_transform.hpp>

#include "../UniformBuffers/UniformBuffers.hpp"
#include "../../OpenGL/FrameBuffer/FrameBuffer.hpp"

#include "../Renderer3D/Renderer3D.hpp"

namespace Pine
{

	class CRenderer2D : public IRenderer2D
	{
	private:
		FrameBuffer* m_TargetFrameBuffer = nullptr;

		VertexArray* m_ScreenQuad = nullptr;

		Shader* m_RectShader = nullptr;

		glm::mat4 m_ProjectionMatrix;

		UniformVariable* m_TransformUniformVariable;
	public:

		void Setup( ) override
		{
			m_RectShader = Assets->GetAsset<Pine::Shader>( "Assets\\Engine\\Shaders\\2D\\Rect.shr" );

			// Setup the quad vertex array
			m_ScreenQuad = new Pine::VertexArray(  );

			m_ScreenQuad->Create( );
			m_ScreenQuad->Bind( );

			const std::vector<float> quads =
			{
				-1.f, 1.f, 0.f,
				-1.f, -1.f, 0.f,
				1.f, -1.f, 0.f,
				1.f, 1.f, 0.f,
			};

			const std::vector<int> indices =
			{
				0,1,3,
				3,1,2
			};

			const std::vector<float> uvs =
			{
				0,0,
				0,1,
				1,1,
				1,0
			};

			m_ScreenQuad->StoreFloatBuffer( quads, 3, 0 );
			m_ScreenQuad->StoreFloatBuffer( uvs, 2, 2 );
			m_ScreenQuad->StoreIntBuffer( indices );
		}
		
		void Dispose( ) override
		{
			m_ScreenQuad->Dispose( );

			delete m_ScreenQuad;
		}

		void PrepareSceneRendering( ) override
		{
			if ( m_TargetFrameBuffer == nullptr ) return;

			m_TargetFrameBuffer->Bind( );

			glViewport( 0, 0, m_TargetFrameBuffer->GetWidth( ), m_TargetFrameBuffer->GetHeight( ) );

			glClearColor( 0.06f, 0.06f, 0.06f, 1.00f );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			glDisable( GL_DEPTH_TEST );
		
			m_ProjectionMatrix = glm::ortho( 0.f, static_cast<float>( m_TargetFrameBuffer->GetWidth( ) ) * 1, 0.f, static_cast< float >( m_TargetFrameBuffer->GetHeight( ) ), -1.f, 1.f );

			UniformBuffers::GetMatrixBufferData( )->ProjectionMatrix = m_ProjectionMatrix;

			UniformBuffers::GetMatrixUniformBuffer( )->Bind( );
			UniformBuffers::GetMatrixUniformBuffer( )->UploadData( 0, sizeof( UniformBuffers::MatrixBufferData_t ), UniformBuffers::GetMatrixBufferData( ) );

			UniformBuffers::GetMaterialUniformBuffer( )->Bind( );
		}

		void SetTargetFrameBuffer( Pine::FrameBuffer* buffer ) override
		{
			m_TargetFrameBuffer = buffer;
		}

		void RenderFilledRect( int x, int y, int w, int h, glm::vec4 color ) override
		{

		}

		void RenderFilledRectTextured( int x, int y, int w, int h, Pine::Texture2D* texture ) override
		{
			// Use the correct vertex array and shader
			m_RectShader->Use( );
			m_ScreenQuad->Bind( );

			//m_RectShader->GetUniformVariable( "projection" )->LoadMatrix4( m_ProjectionMatrix );

			// Set the texture
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, texture->GetId( ) );

			Renderer3D->UpdateTextureBound( 0, texture->GetId( ) );

			// Apply the material
			const auto materialDataBuffer = UniformBuffers::GetMaterialBufferData( );

			materialDataBuffer->diffuseColor = glm::vec3( 1.f );

			UniformBuffers::GetMaterialUniformBuffer( )->UploadData( 0, sizeof( UniformBuffers::MaterialBufferData_t ), materialDataBuffer );

			x += w * 0.5f;
			y += h * 0.5f;

			w /= 2;
			h /= 2;

			y = static_cast< float >( m_TargetFrameBuffer->GetHeight( ) ) - y;

			// Update transformation matrix
			glm::mat4 transformationMatrix = glm::mat4( 1.0f );

			transformationMatrix = glm::translate( transformationMatrix, glm::vec3( x, y, 0.f ) );

			transformationMatrix = glm::translate( transformationMatrix, glm::vec3( 0.5f * static_cast<float>( w ), 0.5f * static_cast< float >( h ), 0.0f ) );
			transformationMatrix = glm::translate( transformationMatrix, glm::vec3( -0.5f * static_cast< float >( w ), -0.5f * static_cast< float >( h ), 0.0f ) );

			transformationMatrix = glm::scale( transformationMatrix, glm::vec3( w, h, 1.0f ) );

			m_TransformUniformVariable = m_RectShader->GetUniformVariable( "transform" );
			m_TransformUniformVariable->LoadMatrix4( transformationMatrix );

			// Render
			glDrawElements( GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr );
		}

	};

	IRenderer2D* CreateRenderer2DInterface( )
	{
		return new CRenderer2D;
	}
}
