#pragma once
#include <string>
#include <vector>
#include <memory>

namespace Pine::Input {

	enum class Axis {
		None = 0,
		MouseX,
		MouseY
	};

	namespace Internal {
		struct KeyboardBinding_t {
			int Key = 0;
			float ActivationValue = 0.f;
		};

		struct AxisBinding_t {
			Axis Axis = Axis::None;
			float Sensitivity = 0.f;
		};
	}

	class InputBinding {
	private:
		std::string m_Name = "";
		float m_Value = 0.f;

		std::vector<std::unique_ptr<Internal::KeyboardBinding_t>> m_KeyboardBindings;
		std::vector<std::unique_ptr<Internal::AxisBinding_t>> m_AxisBindings;
	public:
		InputBinding( const std::string& name );

		void AddKeyboardBinding( int key, float value );
		void AddAxisBinding( Axis axis, float sensitivity );

		void DeleteKeyboardBinding( int i );
		void DeleteAxisBinding( int i );

		std::string& Name( );
		float& Value( );

		const std::vector<std::unique_ptr<Pine::Input::Internal::KeyboardBinding_t>>& GetKeyboardBindings( );
		const std::vector<std::unique_ptr<Pine::Input::Internal::AxisBinding_t>>& GetAxisBindings( );
	};

	InputBinding* CreateBinding( const std::string& name );
	void RemoveBinding( InputBinding* binding );

	// To loop through bindings if you need that.
	int BindingCount( );
	InputBinding* GetBindingById( int i );

	// Please, don't call each frame, call once and store it. It won't get removed, if you don't do it yourself!
	InputBinding* FindBinding( const std::string& name );

	// Called internally by the engine
	void Update( );

	// Saves all input bindings to a file
	void Save( const std::string& file );
	bool Load( const std::string& file );

	// Simple wrappers
	bool IsKeyDown( int key );

}