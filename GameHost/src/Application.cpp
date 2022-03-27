#include <Pine/Pine.hpp>
#include "Pine/GameManager/GameManager.hpp"
#include "Pine/RuntimeLoader/RuntimeLoader.hpp"

int main()
{
    Pine::Window::SetFullscreenMode(true);

    if (!Pine::Setup())
    {
        return 1;
    }

    Pine::Assets->LoadFromDirectory("Projects\\Debug" );
    Pine::RuntimeLoader->LoadModule("Debug\\GameRuntime.dll");
    Pine::GameManager::Load( "Projects\\Debug\\game.asset" );

    Pine::Window::SetSize(1920, 1080);
    Pine::Window::SetPosition(0, 0);

    Pine::Run();

    Pine::Terminate();

    return 0;
}
