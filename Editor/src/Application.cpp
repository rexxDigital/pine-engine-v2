#include <iostream>
#include <Pine/Pine.hpp>
#include "Editor/Editor.hpp"
#include <Pine/Entitylist/EntityList.hpp>
#include <Pine/Components/ModelRenderer/ModelRenderer.hpp>

int main( ) {
	if ( !Pine::Setup( ) ) {
		return 1;
	}

	Editor::Setup( );

	auto e = Pine::EntityList::CreateEntity( );

	e->AddComponent( new Pine::ModelRenderer( ) );

	Pine::Run( );

	Pine::Terminate( );

	return 0;
}


