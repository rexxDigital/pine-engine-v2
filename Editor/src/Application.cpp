#include <iostream>
#include <Pine/Pine.hpp>
#include "Editor/Editor.hpp"

int main( ) {
	if ( !Pine::Setup( ) ) {
		return 1;
	}

	Editor::Setup( );

	Pine::Run( );

	Pine::Terminate( );

	return 0;
}


