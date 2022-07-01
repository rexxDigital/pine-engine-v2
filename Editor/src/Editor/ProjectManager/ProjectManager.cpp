#include "ProjectManager.hpp"
#include <filesystem>
#include <json.hpp>
#include <Pine/Assets/Assets.hpp>
#include <Pine/Core/Window/Window.hpp>

#include "Pine/Pine.hpp"
#include "Pine/Core/Log/Log.hpp"
#include "Pine/RuntimeLoader/RuntimeLoader.hpp"
#include "Pine/Core/Timer/Timer.hpp"

// TODO: Store project meta data, and verify stuff.
// This will probably never get done, lol.

// Don't kill me over this.
void UpdateAssetCache( );

namespace
{

    bool g_LevelOpen = false;
    Pine::Level* g_CurrentLevel = nullptr;

    bool g_ProjectOpen = false;
    std::string g_CurrentProject = "";

    std::vector<std::string> g_AvailableProjects;

    std::chrono::system_clock::duration g_LastProjectRuntimeWriteTime;

    void LoadProject( const std::string& directory )
    {
        g_CurrentProject = directory;
        g_ProjectOpen = true;

        Pine::Log->Message( "Loading project '" + g_CurrentProject + "' assets..." );

        Pine::Timer loadTimer;

        Pine::Assets->LoadFromDirectory( directory );

        loadTimer.Stop();

        Pine::Log->Message("Project assets took " + std::to_string(loadTimer.GetElapsedTimeInMs() / 1000.f) + "s to load.");

        Pine::Window::SetSize( 1024, 768 );
        Pine::Window::UpdateCachedSize( );
    }

}

void Editor::ProjectManager::Setup( )
{
    for ( const auto& dirEntry: std::filesystem::directory_iterator( "Projects" ))
    {
        if ( !dirEntry.is_directory( ) )
            continue;

        g_AvailableProjects.push_back( dirEntry.path( ).string( ) );
    }
}

void Editor::ProjectManager::Save( )
{
    Pine::Log->Message( "Saving all assets..." );

    if ( g_CurrentLevel )
    {
        g_CurrentLevel->CreateFromCurrentLevel( );
        g_CurrentLevel->SetUpdated( true );
    }

    Pine::Assets->SaveAssets( );
}

bool Editor::ProjectManager::HasProjectOpen( )
{
    return g_ProjectOpen;
}

std::string Editor::ProjectManager::GetCurrentProjectDirectory( )
{
    return g_CurrentProject;
}

Pine::Level* Editor::ProjectManager::GetCurrentLevel( )
{
    return g_CurrentLevel;
}

void Editor::ProjectManager::OpenLevel( const std::string& path )
{
    const auto level = Pine::Assets->GetAsset<Pine::Level>( path );
    if ( !level )
        return;

    level->Load( );

    g_CurrentLevel = level;
}

void Editor::ProjectManager::OpenLevel( Pine::Level* level )
{
    g_CurrentLevel = level;
}

void Editor::ProjectManager::SaveLevel( const std::string& path )
{
    g_CurrentLevel = new Pine::Level( );

    g_CurrentLevel->CreateFromCurrentLevel( );
    g_CurrentLevel->SetFilePath( path );

    g_CurrentLevel->SaveToFile( );
}

void Editor::ProjectManager::OpenProject( const std::string& directory )
{
    if ( !std::filesystem::exists( directory ))
        return;

    LoadProject( directory );
}

void Editor::ProjectManager::Update( )
{
    static float lastExecTime = 0.0;

    if ( Pine::GetTime( ) - lastExecTime > 2.f )
    {
        lastExecTime = Pine::GetTime( );
    } else
    {
        return;
    }
}

void Editor::ProjectManager::CreateProject( const std::string& directory )
{
    if ( std::filesystem::exists( directory ))
        return;

    std::filesystem::create_directory( directory );

    LoadProject( directory );
}

const std::vector<std::string>& Editor::ProjectManager::GetAvailableProjects( )
{
    return g_AvailableProjects;
}

void Editor::ProjectManager::ReloadProjectAssets( )
{
    if ( !g_ProjectOpen )
        return;

    const auto level = new Pine::Level;

    level->CreateFromCurrentLevel( );

    Pine::Assets->LoadFromDirectory( GetCurrentProjectDirectory( ));

    level->Load( );
    level->Dispose( );

    delete level;

    UpdateAssetCache( );
}
