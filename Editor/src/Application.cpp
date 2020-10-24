#include <iostream>
#include <Pine/Pine.hpp>
#include "Editor/Editor.hpp"
#include <Pine/Entitylist/EntityList.hpp>
#include <Pine/Components/ModelRenderer/ModelRenderer.hpp>

int main( ) {
	if ( !Pine::Setup( ) ) {
		return 1;
	}

	Pine::Window::SetSize( 700, 400 );

	Editor::Setup( );

	Pine::Run( );

	Pine::Terminate( );

	return 0;
}


