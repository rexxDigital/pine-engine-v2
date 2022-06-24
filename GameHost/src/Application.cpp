#include <Pine/Pine.hpp>
#include "Pine/GameManager/GameManager.hpp"
#include "Pine/RuntimeLoader/RuntimeLoader.hpp"
#include "Pine/Assets/Level/Level.hpp"
#include "Pine/Rendering/RenderManager/RenderManager.hpp"
#include "Pine/Entitylist/EntityList.hpp"

int main()
{
    Pine::Window::SetFullscreenMode(false );

    if ( !Pine::Setup( ) )
    {
        return 1;
    }

    Pine::Window::SetSize(1280, 720);

    //Pine::Assets->LoadFromDirectory("Projects\\Debug" );
    //Pine::RuntimeLoader->LoadModule("Debug\\GameRuntime.dll");
    //Pine::GameManager::Load( "Projects\\Debug\\game.asset" );

    auto player = Pine::EntityList->CreateEntity("Player");

    player->AddComponent(Pine::ComponentType::Camera);

    Pine::RenderManager->GetRenderingContext( )->m_Camera = player->GetComponent<Pine::Camera>();

    Pine::Run( );

    Pine::Terminate( );

    return 0;
}
