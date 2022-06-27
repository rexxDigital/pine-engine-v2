#include "ShadowManager.hpp"
#include "../../OpenGL/FrameBuffer/FrameBuffer.hpp"
#include "../../Components/Camera/Camera.hpp"
#include "../Renderer3D/Renderer3D.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../../Entity/Entity.hpp"

#include <GL/glew.h>

constexpr int ShadowMapResolution = 512;

namespace Pine
{

    struct LightData_t
    {
        Light* m_Light = nullptr;

    };

    glm::mat4 CalculateViewMatrix( const glm::vec3& transform, const glm::vec3& rotation )
    {
        const float verticalAngle = glm::radians( rotation.x );
        const float horizontalAngle = glm::radians( rotation.y );

        const glm::vec3 direction(
                std::cos( verticalAngle ) * std::sin( horizontalAngle ),
                std::sin( verticalAngle ),
                std::cos( verticalAngle ) * std::cos( horizontalAngle )
        );

        const glm::vec3 right = glm::vec3(
                std::sin( horizontalAngle - 3.14159265359f / 2.0f ),
                0,
                std::cos( horizontalAngle - 3.14159265359f / 2.0f )
        );

        const glm::vec3 up = glm::cross( right, direction );

        return glm::lookAt( transform, transform + direction, up );
    }

    class CShadowManager : public IShadowManager {
    private:

        Pine::FrameBuffer* m_DirectionalLightMap = nullptr;

        std::vector<Light*> m_Lights;

        Pine::Shader* m_ShadowShader = nullptr;

        glm::mat4 m_ShadowProjectionMatrix;

    public:

        void Setup( ) override
        {
            m_DirectionalLightMap = new FrameBuffer( );
            m_DirectionalLightMap->Create( ShadowMapResolution, ShadowMapResolution, Buffers::DepthBuffer );

            m_ShadowProjectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.f, 10.f);

            m_ShadowShader = Pine::Assets->GetAsset<Pine::Shader>("Assets/Engine/Shaders/Shadow.shr");
        }

        void Dispose( ) override
        {
            if ( m_DirectionalLightMap )
                m_DirectionalLightMap->Dispose( );

            delete m_DirectionalLightMap;
        }

        void PrepareScene( ) override
        {
            m_Lights.clear( );
        }

        void PrepareLight( Pine::Light* light ) override
        {
            if ( !light->GetCastShadows( ) )
                return;

            m_Lights.push_back( light );
        }

        void Render( std::unordered_map<Model*, std::vector<ModelRenderer*>>& renderBatch ) override
        {
            glEnable( GL_DEPTH_TEST );

            for ( auto light : m_Lights )
            {
                if ( light->GetLightType( ) == LightType::Directional )
                {
                    m_DirectionalLightMap->Bind( );

                    glClear(GL_DEPTH_BUFFER_BIT);
                    glViewport(0, 0, ShadowMapResolution, ShadowMapResolution);

                    glm::mat4 viewMatrix;

                    Renderer3D->UploadCameraData( m_ShadowProjectionMatrix, CalculateViewMatrix( light->GetParent( )->GetTransform( )->GetPositionSum( ), light->GetParent( )->GetTransform( )->GetRotationSum( ) ) );
                    Renderer3D->SetShader( m_ShadowShader );



                }
            }
        }

    };

    IShadowManager* CreateShadowManager( )
    {
        return new CShadowManager( );
    }
}