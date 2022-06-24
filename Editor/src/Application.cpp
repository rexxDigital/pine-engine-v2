#include <iostream>
#include <Pine/Pine.hpp>
#include "Editor/Editor.hpp"

int main( )
{
	if ( !Pine::Setup( ) )
	{
		return 1;
	}

	Pine::Window::SetSize( 1280, 720 );

	Editor::Setup( );

	Pine::Run( );

	Pine::Terminate( );

	return 0;
}