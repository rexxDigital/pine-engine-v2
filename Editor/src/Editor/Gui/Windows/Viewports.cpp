#include <GLFW/glfw3.h>

#include "Windows.hpp"
#include "../Gui.hpp"
#include "../../RenderingHandler/RenderingHandler.hpp"

#include <Pine/OpenGL/FrameBuffer/FrameBuffer.hpp>

#include <ImGuizmo/ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>


#include "Editor/EditorEntity/EditorEntity.hpp"
#include "Pine/Rendering/RenderManager/RenderManager.hpp"

#include "Pine/Entity/Entity.hpp"
#include "Pine/Components/Transform/Transform.hpp"

#include "Editor/PlayManager/PlayManager.hpp"
#include "Pine/Entitylist/EntityList.hpp"

#include "Editor/Gui/Utility/HotkeyManager/HotkeyManager.hpp"
#include "Editor/ProjectManager/ProjectManager.hpp"
#include "Pine/Pine.hpp"
#include "Pine/Components/ModelRenderer/ModelRenderer.hpp"
#include "Pine/Core/Math/Math.hpp"
#include "Pine/Rendering/DebugOverlay/DebugOverlay.hpp"
#include "Pine/Assets/Texture3D/Texture3D.hpp"
#include "Pine/Input/Input.hpp"
#include "Pine/Rendering/Skybox/Skybox.hpp"

namespace
{
    // https://stackoverflow.com/questions/17918033/glm-decompose-mat4-into-translation-and-rotation
    void DecomposeMatrix( const glm::mat4& m, glm::vec3& pos, glm::quat& rot, glm::vec3& scale )
    {
        pos = m[ 3 ];
        for ( int i = 0; i < 3; i++ )
            scale[ i ] = glm::length( glm::vec3( m[ i ] ) );
        const glm::mat3 rotMtx(
                glm::vec3( m[ 0 ] ) / scale[ 0 ],
                glm::vec3( m[ 1 ] ) / scale[ 1 ],
                glm::vec3( m[ 2 ] ) / scale[ 2 ] );
        rot = glm::quat_cast( rotMtx );
    }

    bool g_StartedPlaying = false;
    bool g_StoppedPlaying = false;

    void ShowViewportControls( const bool inLevelViewport, ImVec2 cursorPos, ImVec2 avSize )
    {
        static auto transformIcon = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\transform.png" );
        static auto rotateIcon = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\rotate.png" );
        static auto scaleIcon = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\scale.png" );

        static auto playIcon = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\play.png" );
        static auto stopIcon = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\stop.png" );

        using namespace Editor::Gui;

        static auto renderIconButton = [ ]( bool active, const Pine::Texture2D* texture ) {
            ImGui::PushStyleColor( ImGuiCol_Button,
                                   active ? ImVec4( 0.26f, 0.48f, 0.35f, 1.0f ) : ImVec4( 0.26f, 0.78f, 0.35f, 1.0f ) );
            const bool ret = ImGui::ImageButton( reinterpret_cast< ImTextureID >( texture->GetId( ) ),
                                                 ImVec2( 16.f, 16.f ) );
            ImGui::PopStyleColor( );
            return ret;
        };

        if ( inLevelViewport )
        {
            ImGui::SetCursorScreenPos( ImVec2( cursorPos.x + avSize.x - 102, cursorPos.y + 5 ) );

            if ( renderIconButton( Globals::SelectedGizmoMovementType == GizmoMovementType::Move, transformIcon ) )
                Globals::SelectedGizmoMovementType = GizmoMovementType::Move;

            ImGui::SameLine( 0, 0 );

            if ( renderIconButton( Globals::SelectedGizmoMovementType == GizmoMovementType::Rotate, rotateIcon ) )
                Globals::SelectedGizmoMovementType = GizmoMovementType::Rotate;

            ImGui::SameLine( 0, 0 );

            if ( renderIconButton( Globals::SelectedGizmoMovementType == GizmoMovementType::Scale, scaleIcon ) )
                Globals::SelectedGizmoMovementType = GizmoMovementType::Scale;

            ImGui::SetCursorScreenPos( ImVec2( cursorPos.x + avSize.x - 142, cursorPos.y + 5 ) );
        } else
        {
            ImGui::SetCursorScreenPos( ImVec2( cursorPos.x + avSize.x - 38, cursorPos.y + 5 ) );
        }

        const bool isPlaying = Editor::PlayManager::IsPlaying( );

        if ( renderIconButton( false, isPlaying ? stopIcon : playIcon ) )
        {
            if ( isPlaying )
            {
                Editor::PlayManager::Stop( );

                g_StoppedPlaying = true;

                Globals::SelectedEntityPtrs.clear( );
            } else
            {
                Editor::PlayManager::Start( );

                g_StartedPlaying = true;
            }
        }
    }

    void HandleAssetViewportDrop( )
    {
        if ( ImGui::BeginDragDropTarget( ) )
        {
            if ( const auto payload = ImGui::AcceptDragDropPayload( "Asset", 0 ) )
            {
                const auto asset = *static_cast< Pine::IAsset** >( payload->Data );

                if ( asset->GetType( ) == Pine::AssetType::Level )
                {
                    if ( const auto level = dynamic_cast< Pine::Level* >( asset ) )
                    {
                        level->Load( );

                        Editor::ProjectManager::OpenLevel( level );
                    }
                } else if ( asset->GetType( ) == Pine::AssetType::Model )
                {
                    // Create the model in front of the camera.
                    if ( const auto model = dynamic_cast< Pine::Model* >( asset ) )
                    {
                        const auto entity = Pine::EntityList->CreateEntity( );
                        const auto camTransform = Editor::EditorEntity::GetEntity( )->GetTransform( );

                        entity->AddComponent( Pine::ComponentType::ModelRenderer );

                        entity->GetComponent<Pine::ModelRenderer>( )->SetModel( model );

                        entity->GetTransform( )->Position =
                                camTransform->Position + (camTransform->GetForward( ) * 20.f);
                    }
                } else if ( asset->GetType( ) == Pine::AssetType::Texture3D )
                {
                    if ( const auto texture3D = dynamic_cast< Pine::Texture3D* >( asset ) )
                    {
                        Pine::Skybox->SetSkyboxCubemap( texture3D );
                    }
                } else if ( asset->GetType( ) == Pine::AssetType::Blueprint )
                {
                    if ( const auto blueprint = dynamic_cast< Pine::Blueprint* >( asset ) )
                    {
                        auto e = blueprint->SpawnEntity( );
                        const auto camTransform = Editor::EditorEntity::GetEntity( )->GetTransform( );

                        e->GetTransform( )->Position = camTransform->Position + (camTransform->GetForward( ) * 20.f);
                    }
                }
            }

            ImGui::EndDragDropTarget( );
        }
    }

    void RenderEntityIcon( const Pine::Entity* entity, Pine::Camera* camera, ImVec2 screenPosition )
    {
        constexpr float IconSize = 32.f;

        static auto cameraIcon = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\camera.png" );
        static auto lightIcon = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\light-bulb.png" );

        // Find the correct icon

        Pine::Texture2D* renderIcon = nullptr;

        for ( const auto component: entity->GetComponents( ) )
        {
            switch ( component->GetType( ) )
            {
                case Pine::ComponentType::Camera:
                    renderIcon = cameraIcon;
                    break;
                case Pine::ComponentType::Light:
                    renderIcon = lightIcon;
                    break;
                default:
                    break;
            }
        }

        if ( !renderIcon )
            return;

        // Calculate the screen coordinates
        auto res = Pine::Math->WorldToScreen( entity->GetTransform( )->GetPositionSum( ), camera );

        if ( res.z > 1.f ) // z > 1.f == Out of bounds
            return;

        // To keep the icon centered
        res -= IconSize * 0.5f;

        ImGui::GetWindowDrawList( )->AddImage( reinterpret_cast< ImTextureID >( renderIcon->GetId( ) ),
                                               ImVec2( screenPosition.x + res.x, screenPosition.y + res.y ),
                                               ImVec2( screenPosition.x + res.x + IconSize,
                                                       screenPosition.y + res.y + IconSize ) );
    }

}

void Editor::Gui::Windows::RenderViewports( )
{
    bool gameWindowOpen = false;

    // --- Game viewport ---

    if ( ShowGameViewport )
    {
        if ( g_StartedPlaying )
        {
            const auto& io = ImGui::GetIO( );

            if ( !io.KeyAlt )
                ImGui::SetNextWindowFocus( );

            g_StartedPlaying = false;
        }

        if ( ImGui::Begin( "Game", &ShowGameViewport, 0 ) )
        {
            gameWindowOpen = true;
            Globals::IsInLevelView = false;

            //ShowViewportControls( false );

            const auto avSize = ImGui::GetContentRegionAvail( );
            const auto cursorScreen = ImGui::GetCursorScreenPos( );

            if ( Pine::RenderManager->GetRenderingContext( )->m_Camera != nullptr )
            {
                ImGui::Image( reinterpret_cast< ImTextureID >( RenderingHandler::GetFrameBuffer( )->GetTextureId( ) ),
                              avSize, ImVec2( 0.f, 0.f ), ImVec2( 1.f, 1.f ) );

                ShowViewportControls( false, cursorScreen, avSize );
            } else
            {
                ImGui::TextColored( ImVec4( 1.f, 0.5f, 0.f, 1.f ),
                                    "No active camera, please make at least one is active for this level." );
            }

            if ( Pine::Input->IsKeyPressed( 256 ) ) // Escape key
            {
                Pine::Input->SetCursorAutoCenter( false );
                Pine::Input->SetCursorVisible( true );
            }

            Pine::DebugOverlay->SetViewport( cursorScreen.x, cursorScreen.y, avSize.x, avSize.y );
            Pine::DebugOverlay->Render( );
        }

        ImGui::End( );
    }


    // --- Level viewport ---

    if ( g_StoppedPlaying )
    {
        const auto& io = ImGui::GetIO( );

        if ( !io.KeyAlt )
            ImGui::SetNextWindowFocus( );

        g_StoppedPlaying = false;
    }

    if ( ShowLevelViewport )
    {
        if ( ImGui::Begin( "Level", &ShowLevelViewport, 0 ) )
        {
            Globals::IsInLevelView = true;

            const auto avSize = ImGui::GetContentRegionAvail( );

            RenderingHandler::SetViewportSize( avSize.x, avSize.y );

            const ImVec2 cursorPos = ImGui::GetCursorScreenPos( );

            ImGuizmo::SetRect( cursorPos.x, cursorPos.y, avSize.x, avSize.y );

            ImGui::Image( reinterpret_cast< ImTextureID >( RenderingHandler::GetFrameBuffer( )->GetTextureId( ) ),
                          avSize, ImVec2( 0.f, 0.f ), ImVec2( 1.f, 1.f ) );

            char buff[32];
            sprintf_s( buff, "FPS: %d", Pine::GetFPS( ) );
            ImGui::GetForegroundDrawList( )->AddText( ImVec2( cursorPos.x + 5, cursorPos.y + 5 ),
                                                      ImColor( 255, 255, 255, 130 ), buff );

            Pine::DebugOverlay->SetViewport( cursorPos.x, cursorPos.y, avSize.x, avSize.y );
            Pine::DebugOverlay->Render( );

            Globals::IsHoveringLevelView = ImGui::IsItemHovered( );

            HandleAssetViewportDrop( );

            ShowViewportControls( true, cursorPos, avSize );

            const auto cam = EditorEntity::GetCamera( );

            // Render ImGuizmo

            if ( !Globals::SelectedEntityPtrs.empty( ) )
            {
                const auto e = Globals::SelectedEntityPtrs[ 0 ];

                if ( cam != nullptr )
                {
                    ImGuizmo::OPERATION op;

                    if ( Globals::SelectedGizmoMovementType == GizmoMovementType::Move )
                        op = ImGuizmo::OPERATION::TRANSLATE;
                    if ( Globals::SelectedGizmoMovementType == GizmoMovementType::Rotate )
                        op = ImGuizmo::OPERATION::ROTATE;
                    if ( Globals::SelectedGizmoMovementType == GizmoMovementType::Scale )
                        op = ImGuizmo::OPERATION::SCALE;

                    ImGuizmo::SetRect( cursorPos.x, cursorPos.y, avSize.x, avSize.y );
                    ImGuizmo::SetDrawlist( ImGui::GetWindowDrawList( ) );

                    // Because ImGuizmo has problems respecting the max view port width and height for whatever reason.
                    ImGui::GetWindowDrawList( )->PushClipRect( ImVec2( cursorPos.x, cursorPos.y ),
                                                               ImVec2( cursorPos.x + avSize.x,
                                                                       cursorPos.y + avSize.y ) );

                    glm::mat4 temp = e->GetTransform( )->GetTransformationMatrix( );

                    if ( Manipulate( glm::value_ptr( cam->GetViewMatrix( ) ),
                                     glm::value_ptr( cam->GetProjectionMatrix( ) ), op, ImGuizmo::WORLD,
                                     glm::value_ptr( temp ), nullptr, nullptr ) )
                    {
                        glm::vec3 translation;
                        glm::quat rotation;
                        glm::vec3 scale;

                        DecomposeMatrix( temp, translation, rotation, scale );

                        auto base_position = glm::vec3( 0.f );

                        if ( e->GetParent( ) != nullptr )
                            base_position = e->GetParent( )->GetTransform( )->Position;

                        e->GetTransform( )->Position =
                                glm::vec3( translation[ 0 ], translation[ 1 ], translation[ 2 ] ) - base_position;
                        e->GetTransform( )->Rotation = glm::degrees( glm::eulerAngles( rotation ) );
                        e->GetTransform( )->Scale = glm::vec3( scale[ 0 ], scale[ 1 ], scale[ 2 ] );
                    }

                    ImGui::GetWindowDrawList( )->PopClipRect( );
                }
            }

            ImGui::GetWindowDrawList( )->PushClipRect( ImVec2( cursorPos.x, cursorPos.y ),
                                                       ImVec2( cursorPos.x + avSize.x, cursorPos.y + avSize.y ) );

            for ( int i = 0; i < Pine::EntityList->GetEntities( ).size( ); i++ )
            {
                const auto entity = Pine::EntityList->GetEntities( )[ i ];

                if ( !entity->GetActive( ) )
                    continue;
                if ( entity == EditorEntity::GetEntity( ) )
                    continue;
                if ( !Globals::SelectedEntityPtrs.empty( ) && Globals::SelectedEntityPtrs[ 0 ] == entity )
                    continue;

                RenderEntityIcon( entity, cam, cursorPos );
            }

            ImGui::GetWindowDrawList( )->PopClipRect( );
        }

        ImGui::End( );
    }

}