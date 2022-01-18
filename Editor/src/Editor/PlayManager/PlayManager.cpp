#include "PlayManager.hpp"

#include "Pine/Pine.hpp"
#include "Pine/Entitylist/EntityList.hpp"

namespace
{
	bool g_IsPlaying = false;
	bool g_ShouldRestoreLevel = true;
	
	Pine::Level* g_BackupLevel = nullptr;
}

void Editor::PlayManager::SetShouldRestoreLevel( bool value )
{
	g_ShouldRestoreLevel = value;
}

bool Editor::PlayManager::GetShouldRestoreLevel( )
{
	return g_ShouldRestoreLevel;
}

bool Editor::PlayManager::IsPlaying( )
{
	return g_IsPlaying;
}

void Editor::PlayManager::Start( )
{
	assert( !g_IsPlaying );

	if ( !g_BackupLevel )
		g_BackupLevel = new Pine::Level;

	g_BackupLevel->CreateFromCurrentLevel( );
	g_IsPlaying = true;

	Pine::SetAllowUpdates( true );

	Pine::EntityList->RunOnSetup( );

	Pine::Window::SetTitle( "Pine - Running" );
}

void Editor::PlayManager::Pause( )
{
	Pine::SetAllowUpdates( !Pine::IsAllowingUpdates(  ) );
}

void Editor::PlayManager::Stop( )
{
	assert( g_IsPlaying );

	Pine::SetAllowUpdates( false );
	
	if ( g_ShouldRestoreLevel )
	{
		g_BackupLevel->Load( );
	}

	g_BackupLevel->DisposeBlueprints( );

	g_IsPlaying = false;

	Pine::Window::SetTitle( "Pine" );
}

Pine::Level* Editor::PlayManager::GetBackupLevel( )
{
	return g_BackupLevel;
}
