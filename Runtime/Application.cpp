#include <iostream>
#include <string>
#include <Windows.h>
#include <Pine/Pine.hpp>

#include "Pine/Assets/Level/Level.hpp"

// As of right now, we don't have any compression and stuff for assets, so we are straight loading em from directories.

int main( )
{
	if ( !Pine::Setup( ) )
		return 0;

	// By default we'll call it the game directory for game assets.
	Pine::Assets::LoadFromDirectory( "Projects" );

	Pine::Assets::GetAsset<Pine::Level>( "Projects\\Default\\cbdf.lvl" )->Load( );

	Pine::Run( );

	Pine::Terminate( );

	return 0;
}