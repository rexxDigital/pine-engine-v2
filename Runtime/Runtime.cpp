#include <iostream>
#include <string>
#include <Windows.h>
#include <Pine/Pine.hpp>

// As of right now, we don't have any compression and stuff for assets, so we are straight loading em from directories.

std::string blahblabhlah[ ] = { "dsdsdsd", "sdsd", "sdds" };

int main( )
{
	if ( !Pine::Setup( ) )
		return 0;

	// By default we'll call it the game directory for game assets.
	Pine::Assets::LoadFromDirectory( "Game" );

	std::cout <<
		ARRAYSIZE( blahblabhlah )
	<< std::endl;
	
	Pine::Run( );

	Pine::Terminate( );

	return 0;
}