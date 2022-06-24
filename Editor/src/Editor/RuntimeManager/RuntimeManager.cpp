#include "RuntimeManager.hpp"
#include "Editor/PlayManager/PlayManager.hpp"
#include <Pine/Core/Log/Log.hpp>
#include <Pine/Core/Window/Window.hpp>
#include <Pine/Input/Input.hpp>

#include <vector>

namespace
{
    std::string g_ProjectRuntimePath = "libGameRuntime.so";
    Pine::ModuleHandle* g_ProjectRuntime = nullptr;

    std::string g_RuntimeSourcePath = "";
    bool g_RuntimeSourceAutoCompile = false;

    std::vector<std::pair<std::filesystem::path, std::chrono::system_clock::duration>> g_SourceFiles;

    void ReloadRuntimeLibrary( const std::filesystem::path& path )
    {
        Pine::Log->Message( "Reloading game runtime library..." );

        if ( g_ProjectRuntime )
        {
            Pine::RuntimeLoader->UnloadModule( g_ProjectRuntime );
            g_ProjectRuntime = nullptr;
        }

        g_ProjectRuntime = Pine::RuntimeLoader->LoadModule( path );
    }

    bool GetSourceFilesUpdated( )
    {
        if ( g_SourceFiles.empty( ) )
            return false;

        bool updated = false;

        for ( auto& pair: g_SourceFiles )
        {
            if ( std::filesystem::last_write_time( pair.first ).time_since_epoch( ) != pair.second )
            {
                updated = true;
                break;
            }
        }

        return updated;
    }

    void BuildSourceFilesList( )
    {
        g_SourceFiles.clear( );

        if ( std::filesystem::exists( g_RuntimeSourcePath + "/src" ) )
        {
            for ( auto item: std::filesystem::recursive_directory_iterator( g_RuntimeSourcePath + "/src" ) )
            {
                if ( item.is_directory( ) )
                    continue;

                g_SourceFiles.push_back( std::make_pair( item.path( ), std::filesystem::last_write_time(
                        item.path( ) ).time_since_epoch( ) ) );
            }
        }
    }

    void OnEditorFocus( )
    {
        if ( !g_RuntimeSourceAutoCompile )
            return;
        if ( Editor::PlayManager::IsPlaying( ) )
            return;

        // Check if files has changed
        if ( !GetSourceFilesUpdated( ) )
            return;

        BuildSourceFilesList( );

        Editor::RuntimeManager::UnloadRuntimeLibrary( );

        if ( Editor::RuntimeManager::CompileRuntime( ) )
        {
            Editor::RuntimeManager::LoadRuntimeLibrary( );
        }
    }
}

const std::string& Editor::RuntimeManager::GetRuntimeLibraryPath( )
{
    return g_ProjectRuntimePath;
}

void Editor::RuntimeManager::SetRuntimeLibraryPath( const std::string& path )
{
    g_ProjectRuntimePath = path;
}

void Editor::RuntimeManager::LoadRuntimeLibrary( )
{
    ReloadRuntimeLibrary( g_ProjectRuntimePath );
}

void Editor::RuntimeManager::UnloadRuntimeLibrary( )
{
    if ( g_ProjectRuntime )
    {
        Pine::RuntimeLoader->UnloadModule( g_ProjectRuntime );
        g_ProjectRuntime = nullptr;
    }
}

Pine::ModuleHandle* Editor::RuntimeManager::GetProjectRuntimeLibrary( )
{
    return g_ProjectRuntime;
}

const std::string& Editor::RuntimeManager::GetRuntimeSourcePath( )
{
    return g_RuntimeSourcePath;
}

void Editor::RuntimeManager::SetRuntimeSourcePath( const std::string& path )
{
    g_RuntimeSourcePath = path;
}

void Editor::RuntimeManager::SetRuntimeAutoCompile( bool enabled )
{
    g_RuntimeSourceAutoCompile = enabled;
}

bool Editor::RuntimeManager::GetRuntimeAutoCompile( )
{
    return g_RuntimeSourceAutoCompile;
}

bool Editor::RuntimeManager::GetRuntimeCompileAvailable( )
{
    return !g_RuntimeSourcePath.empty( );
}

bool Editor::RuntimeManager::CompileRuntime( )
{
    // This is "temporary", hence why it's stupid as fuck.
    // Probably unsafe as well, don't care though.

    Pine::Log->Message( "Compiling runtime..." );

    if ( !std::filesystem::exists( g_RuntimeSourcePath + "/src" ) )
    {
        Pine::Log->Error( "Failed to compile run time, can't find src/" );
        return false;
    }

    if ( !std::filesystem::exists( "../CMakeLists.txt" ) )
    {
        Pine::Log->Error( "Failed to compile run time, can't find CMakeLists.txt" );
        return false;
    }

    /*
    if ( !std::filesystem::exists( "../bin-runtime" ) )
    {
        Pine::Log->Message( "Setting up make files..." );

        std::filesystem::create_directory( "../bin-runtime" );

        if ( std::system( "cd ../bin-runtime && cmake .." ) != 0 )
        {
            Pine::Log->Error( "Failed to setup make files." );
            return false;
        }
    }
    */

    if ( std::system( "cd ../cmake-build-debug && cmake --build . --target GameRuntime" ) != 0 )
    {
        Pine::Log->Error( "Failed to compile game runtime." );
        return false;
    }

    Pine::Log->Message( "Compiled runtime successfully!" );

    return true;
}

void Editor::RuntimeManager::Setup( )
{
    g_RuntimeSourcePath = std::filesystem::absolute( "../GameRuntime" ).string( );
    g_RuntimeSourceAutoCompile = true;

    // Setup initial source tree, if possible
    BuildSourceFilesList( );

    LoadRuntimeLibrary( );
}

void Editor::RuntimeManager::Update( )
{
    static bool lastFocus = false;

    if ( Pine::Input->IsWindowFocused( ) != lastFocus )
    {
        lastFocus = Pine::Input->IsWindowFocused( );

        if ( lastFocus )
        {
            OnEditorFocus( );
        }
    }
}

