#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glm/vec2.hpp>

#include "../Core/Interfaces/Interfaces.hpp"

namespace Pine
{

	enum class Axis
	{
		None = 0,
		MouseX,
		MouseY
	};

	struct KeyboardBinding_t
	{
		int Key = 0;
		float ActivationValue = 0.f;
	};

	struct AxisBinding_t
	{
		Axis Axis = Axis::None;
		float Sensitivity = 0.f;
	};

	class InputBinding
	{
	private:
		std::string m_Name = "";
		float m_Value = 0.f;

		std::vector<std::unique_ptr<KeyboardBinding_t>> m_KeyboardBindings;
		std::vector<std::unique_ptr<AxisBinding_t>> m_AxisBindings;
	public:
		InputBinding( const std::string& name );

		void AddKeyboardBinding( int key, float value );
		void AddAxisBinding( Axis axis, float sensitivity );

		void DeleteKeyboardBinding( int i );
		void DeleteAxisBinding( int i );

		std::string& Name( );
		float& Value( );

		const std::vector<std::unique_ptr<KeyboardBinding_t>>& GetKeyboardBindings( );
		const std::vector<std::unique_ptr<AxisBinding_t>>& GetAxisBindings( );
	};

	class IInputSystem : public IInterface
	{
	public:
		virtual InputBinding* CreateBinding( const std::string& name ) = 0;
		virtual void RemoveBinding( InputBinding* binding ) = 0;

		// To loop through bindings if you need that.
		virtual int BindingCount( ) = 0;
		virtual InputBinding* GetBindingById( int i ) = 0;

		// Please don't call each frame, call once and store it. It won't get removed, if you don't do it yourself!
		virtual InputBinding* FindBinding( const std::string& name ) = 0;

		// Called internally by the engine
		virtual void Setup( ) = 0;
		virtual void Update( ) = 0;

		// Saves all input bindings to a file
		virtual void Save( const std::string& file ) = 0;
		virtual bool Load( const std::string& file ) = 0;

		// Simple wrappers
		virtual bool IsKeyDown( int key ) = 0;
		virtual bool IsKeyPressed( int key ) = 0;
		virtual bool IsKeyReleased( int key ) = 0;

        // If the window is focused within the OS
        virtual bool IsWindowFocused( ) = 0;

        virtual void SetCursorAutoCenter( bool enabled ) = 0;
        virtual void SetCursorVisible( bool enabled ) = 0;
        virtual void SetIgnoreInput( bool enabled ) = 0;
//      virtual void SetIgnoreWhenUnfocused( bool enabled ) = 0;

        virtual bool GetCursorAutoCenter( ) const = 0;
        virtual bool GetCursorVisible( ) const = 0;
        virtual bool GetIgnoreInput( ) const = 0;

        virtual glm::ivec2 GetMousePosition( ) = 0;
		virtual glm::ivec2 GetMouseDelta( ) = 0;
	};

}