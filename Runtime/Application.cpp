#include <iostream>
#include <string>
#include <Windows.h>
#include <Pine/Pine.hpp>

// As of right now, we don't have any compression and stuff for assets, so we are straight loading em from directories.

int main( )
{
	if ( !Pine::Setup( ) )
		return 0;

	// By default we'll call it the game directory for game assets.
	Pine::Assets::LoadFromDirectory( "Game" );
	
	Pine::Run( );

	Pine::Terminate( );

	return 0;
}