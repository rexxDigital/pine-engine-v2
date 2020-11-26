#pragma once
#include "Pine/Assets/Level/Level.hpp"

namespace Editor::PlayManager
{

	void SetShouldRestoreLevel( bool value );
	bool GetShouldRestoreLevel( );
	
	bool IsPlaying( );

	void Start( );
	void Pause( );
	void Stop( );

	Pine::Level* GetBackupLevel( );
	
}
