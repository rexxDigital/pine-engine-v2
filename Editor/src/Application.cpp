#include <iostream>
#include <Pine/Pine.hpp>
#include "Editor/Editor.hpp"
#include <Pine/Entitylist/EntityList.hpp>

int main( ) {
	if ( !Pine::Setup( ) ) {
		return 1;
	}

	Editor::Setup( );

	auto e = Pine::EntityList::CreateEntity( );

	e->CreateChild( );

	Pine::Run( );

	Pine::Terminate( );

	return 0;
}


