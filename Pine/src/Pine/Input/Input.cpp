#include "Input.hpp"

#include <json.hpp>

#include <fstream>
#include <glfw/glfw3.h>
#include "../Core/Window/Window.hpp"

namespace {
	std::vector<std::unique_ptr<Pine::Input::InputBinding>> m_InputBindings;
}

Pine::Input::InputBinding::InputBinding( const std::string& name ) {
	m_Name = name;
}

void Pine::Input::InputBinding::AddKeyboardBinding( const int key, const float value ) {
	auto binding = std::make_unique< Internal::KeyboardBinding_t >( );

	binding->Key = key;
	binding->ActivationValue = value;

	m_KeyboardBindings.push_back( std::move( binding ) );
}

void Pine::Input::InputBinding::AddAxisBinding( const Axis axis, const float sensitivity ) {
	auto binding = std::make_unique< Internal::AxisBinding_t >( );

	binding->Axis = axis;
	binding->Sensitivity = sensitivity;

	m_AxisBindings.push_back( std::move( binding ) );
}

void Pine::Input::InputBinding::DeleteKeyboardBinding( const int i ) {
	m_KeyboardBindings.erase( m_KeyboardBindings.begin( ) + i );
}

void Pine::Input::InputBinding::DeleteAxisBinding( const int i ) {
	m_AxisBindings.erase( m_AxisBindings.begin( ) + i );
}

std::string& Pine::Input::InputBinding::Name( ) {
	return m_Name;
}

float& Pine::Input::InputBinding::Value( ) {
	return m_Value;
}

const std::vector<std::unique_ptr<Pine::Input::Internal::KeyboardBinding_t>>& Pine::Input::InputBinding::GetKeyboardBindings( ) {
	return m_KeyboardBindings;
}

const std::vector<std::unique_ptr<Pine::Input::Internal::AxisBinding_t>>& Pine::Input::InputBinding::GetAxisBindings( ) {
	return m_AxisBindings;
}

Pine::Input::InputBinding* Pine::Input::CreateBinding( const std::string& name ) {
	m_InputBindings.push_back( std::make_unique<Pine::Input::InputBinding>( name ) );

	return m_InputBindings[ m_InputBindings.size( ) - 1 ].get( );
}

void Pine::Input::RemoveBinding( InputBinding* binding ) {
	int i = 0;
	for ( auto& bind : m_InputBindings ) {
		if ( bind.get( ) == binding ) {
			m_InputBindings.erase( m_InputBindings.begin( ) + i );
			return;
		}

		i++;
	}
}

int Pine::Input::BindingCount( ) {
	return m_InputBindings.size( );
}

Pine::Input::InputBinding* Pine::Input::GetBindingById( const int i ) {
	return m_InputBindings[ i ].get( );
}

Pine::Input::InputBinding* Pine::Input::FindBinding( const std::string& name ) {
	for ( auto& bind : m_InputBindings ) {
		if ( bind->Name( ) == name ) {
			return bind.get( );
		}
	}

	return nullptr;
}

void Pine::Input::Update( ) {
	const auto window = Pine::Window::Internal::GetWindowPointer( );

	// Update mouse delta
	double x, y;
	static double lastX, lastY;

	glfwGetCursorPos( window, &x, &y );

	const double mouseX = lastX - x;
	const double mouseY = lastY - y;

	lastX = x;
	lastY = y;

	for ( auto& bind : m_InputBindings ) {
		bind->Value( ) = 0;

		for ( auto& axis : bind->GetAxisBindings( ) ) {
			// Mouse Axis
			if ( axis->Axis == Axis::MouseX ) {
				bind->Value( ) = mouseX * axis->Sensitivity;
			}

			if ( axis->Axis == Axis::MouseY ) {
				bind->Value( ) = mouseY * axis->Sensitivity;
			}
		}

		for ( auto& key : bind->GetKeyboardBindings( ) ) {
			if ( glfwGetKey( window, key->Key ) == GLFW_PRESS ) {
				bind->Value( ) += key->ActivationValue;
			}
		}
	}
}

void Pine::Input::Save( const std::string& file ) {
	std::ofstream stream( file );
	nlohmann::json json;

	for ( int i = 0; i < m_InputBindings.size( ); i++ ) {
		auto binding = m_InputBindings[ i ].get( );

		json[ i ][ "Name" ] = binding->Name( );

		// Write keyboard bindings
		for ( int j = 0; j < binding->GetKeyboardBindings( ).size( ); j++ ) {
			auto keyboard = binding->GetKeyboardBindings( )[ j ].get( );

			json[ i ][ "Keyboard" ][ j ][ "ActivationValue" ] = keyboard->ActivationValue;
			json[ i ][ "Keyboard" ][ j ][ "Key" ] = keyboard->Key;
		}

		// Write axis bindings
		for ( int j = 0; j < binding->GetAxisBindings( ).size( ); j++ ) {
			auto axis = binding->GetAxisBindings( )[ j ].get( );

			json[ i ][ "Axis" ][ j ][ "Axis" ] = axis->Axis;
			json[ i ][ "Axis" ][ j ][ "Sensitivity" ] = axis->Sensitivity;
		}
	}

	stream << json.dump( );
	stream.close( );
}

bool Pine::Input::Load( const std::string& file ) {
	std::ifstream stream( file );

	if ( !stream.is_open( ) )
		return false;

	std::string str( ( std::istreambuf_iterator<char>( stream ) ),
		std::istreambuf_iterator<char>( ) );

	if ( str.empty( ) ) {
		stream.close( );
		return false;
	}

	nlohmann::json json = nlohmann::json::parse( str );

	for ( auto bindingJson : json.items( ) ) {
		auto binding = CreateBinding( bindingJson.value( )[ "Name" ] );

		for ( const auto& keyboardJson : bindingJson.value( )[ "Keyboard" ].items( ) ) {
			binding->AddKeyboardBinding( keyboardJson.value( )[ "Key" ], keyboardJson.value( )[ "ActivationValue" ] );
		}

		for ( const auto& axisJson : bindingJson.value( )[ "Axis" ].items( ) ) {
			binding->AddAxisBinding( static_cast< Axis >( axisJson.value( )[ "Axis" ].get<int>( ) ), axisJson.value( )[ "Sensitivity" ] );
		}
	}

	return true;
}

bool Pine::Input::IsKeyDown( int key ) {
	return false;
}
