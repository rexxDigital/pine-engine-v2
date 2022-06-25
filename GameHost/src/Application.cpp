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

    Pine::Assets->LoadFromDirectory("Projects/Debug" );
    Pine::RuntimeLoader->LoadModule("libGameRuntime.so");
    Pine::GameManager::Load( "Projects/Debug/game.asset" );

    Pine::Run( );

    Pine::Terminate( );

    return 0;
}
