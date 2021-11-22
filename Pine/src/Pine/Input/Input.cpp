#include "Input.hpp"

#include <json.hpp>
#include <fstream>
#include <GLFW/glfw3.h>

#include "../Core/Window/Window.hpp"
#include "../Rendering/DebugOverlay/DebugOverlay.hpp"

namespace Pine
{

	class CInputSystem : public IInputSystem
	{
	private:
		std::vector<std::unique_ptr<InputBinding>> m_InputBindings;

		glm::ivec2 m_MouseDelta;

	public:

		InputBinding* CreateBinding( const std::string& name ) override
		{
			m_InputBindings.push_back( std::make_unique<InputBinding>( name ) );

			return m_InputBindings[ m_InputBindings.size( ) - 1 ].get( );
		}

		void RemoveBinding( InputBinding* binding ) override
		{
			int i = 0;
			for ( auto& bind : m_InputBindings ) {
				if ( bind.get( ) == binding ) {
					m_InputBindings.erase( m_InputBindings.begin( ) + i );
					return;
				}

				i++;
			}
		}

		int BindingCount( ) override
		{
			return m_InputBindings.size( );
		}

		InputBinding* GetBindingById( const int i ) override
		{
			return m_InputBindings[ i ].get( );
		}

		InputBinding* FindBinding( const std::string& name ) override
		{
			for ( const auto& bind : m_InputBindings ) {
				if ( bind->Name( ) == name ) {
					return bind.get( );
				}
			}

			return nullptr;
		}

		void Update( ) override
		{
			const auto window = Pine::Window::Internal::GetWindowPointer( );

			// Update mouse delta
			double x, y;
			static double lastX, lastY;

			glfwGetCursorPos( window, &x, &y );

			const double mouseX = lastX - x;
			const double mouseY = lastY - y;

			lastX = x;
			lastY = y;

			m_MouseDelta = glm::ivec2( mouseX, mouseY );

			for ( const auto& bind : m_InputBindings ) {
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

		void Save( const std::string& file ) override
		{
			std::ofstream stream( file );
			nlohmann::json json;

			for ( int i = 0; i < m_InputBindings.size( ); i++ ) {
				const auto binding = m_InputBindings[ i ].get( );

				json[ i ][ "Name" ] = binding->Name( );

				// Write keyboard bindings
				for ( int j = 0; j < binding->GetKeyboardBindings( ).size( ); j++ ) {
					const auto keyboard = binding->GetKeyboardBindings( )[ j ].get( );

					json[ i ][ "Keyboard" ][ j ][ "ActivationValue" ] = keyboard->ActivationValue;
					json[ i ][ "Keyboard" ][ j ][ "Key" ] = keyboard->Key;
				}

				// Write axis bindings
				for ( int j = 0; j < binding->GetAxisBindings( ).size( ); j++ ) {
					const auto axis = binding->GetAxisBindings( )[ j ].get( );

					json[ i ][ "Axis" ][ j ][ "Axis" ] = axis->Axis;
					json[ i ][ "Axis" ][ j ][ "Sensitivity" ] = axis->Sensitivity;
				}
			}

			stream << json.dump( );
			stream.close( );
		}

		bool Load( const std::string& file ) override
		{
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

			for ( const auto& bindingJson : json.items( ) ) {
				const auto binding = CreateBinding( bindingJson.value( )[ "Name" ] );

				for ( const auto& keyboardJson : bindingJson.value( )[ "Keyboard" ].items( ) ) {
					binding->AddKeyboardBinding( keyboardJson.value( )[ "Key" ], keyboardJson.value( )[ "ActivationValue" ] );
				}

				for ( const auto& axisJson : bindingJson.value( )[ "Axis" ].items( ) ) {
					binding->AddAxisBinding( static_cast< Axis >( axisJson.value( )[ "Axis" ].get<int>( ) ), axisJson.value( )[ "Sensitivity" ] );
				}
			}

			return true;
		}

		bool IsKeyDown( int key ) override
		{
			return false;
		}

		glm::ivec2 GetMousePosition( ) override
		{
			double x, y;

			glfwGetCursorPos( Window::Internal::GetWindowPointer( ), &x, &y );

			// The returned cursor position should always be relative to the game output, so if the viewport position
			// has been changed, like for instance in the editor, we'll have to account for that.
			const auto viewport = Pine::DebugOverlay->GetViewport( );

			x -= viewport.x;
			y -= viewport.y;

			return glm::ivec2( x, y );
		}

		glm::ivec2 GetMouseDelta( ) override
		{
			return m_MouseDelta;
		}

	};

	IInputSystem* CreateInputSystemInterface( )
	{
		return new CInputSystem( );
	}

}


Pine::InputBinding::InputBinding( const std::string& name ) {
	m_Name = name;
}

void Pine::InputBinding::AddKeyboardBinding( const int key, const float value ) {
	auto binding = std::make_unique< KeyboardBinding_t >( );

	binding->Key = key;
	binding->ActivationValue = value;

	m_KeyboardBindings.push_back( std::move( binding ) );
}

void Pine::InputBinding::AddAxisBinding( const Axis axis, const float sensitivity ) {
	auto binding = std::make_unique< AxisBinding_t >( );

	binding->Axis = axis;
	binding->Sensitivity = sensitivity;

	m_AxisBindings.push_back( std::move( binding ) );
}

void Pine::InputBinding::DeleteKeyboardBinding( const int i ) {
	m_KeyboardBindings.erase( m_KeyboardBindings.begin( ) + i );
}

void Pine::InputBinding::DeleteAxisBinding( const int i ) {
	m_AxisBindings.erase( m_AxisBindings.begin( ) + i );
}

std::string& Pine::InputBinding::Name( ) {
	return m_Name;
}

float& Pine::InputBinding::Value( ) {
	return m_Value;
}

const std::vector<std::unique_ptr<Pine::KeyboardBinding_t>>& Pine::InputBinding::GetKeyboardBindings( ) {
	return m_KeyboardBindings;
}

const std::vector<std::unique_ptr<Pine::AxisBinding_t>>& Pine::InputBinding::GetAxisBindings( ) {
	return m_AxisBindings;
}
