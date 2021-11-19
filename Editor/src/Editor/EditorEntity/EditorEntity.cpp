#include "EditorEntity.hpp"

#include <glfw/glfw3.h>
#include <Pine/Components/NativeScript/NativeScript.hpp>

#include "Pine/Components/Camera/Camera.hpp"
#include "Pine/Entitylist/EntityList.hpp"
#include "Pine/Input/Input.hpp"

#include "../Gui/Gui.hpp"
#include "ImGui/imgui.h"

namespace
{
	Pine::Entity* g_EditorEntity = nullptr;
	Pine::Camera* g_Camera = nullptr;
}

using namespace Editor::Gui;

class EditorEntityScript final : public Pine::IComponent
{
private:
	bool m_IsMovingCamera = false;

	Pine::InputBinding* m_Pitch = nullptr;
	Pine::InputBinding* m_Yaw = nullptr;

	Pine::InputBinding* m_Forward = nullptr;
	Pine::InputBinding* m_Sideways = nullptr;
public:

	void OnSetup( ) override
	{
	}

	void OnCreated( ) override
	{
		m_Pitch = Pine::Input->CreateBinding( "Pitch" );
		m_Pitch->AddAxisBinding( Pine::Axis::MouseY, 0.15f );

		m_Yaw = Pine::Input->CreateBinding( "Yaw" );
		m_Yaw->AddAxisBinding( Pine::Axis::MouseX, 0.15f );

		m_Forward = Pine::Input->CreateBinding( "Forward" );
		m_Forward->AddKeyboardBinding( GLFW_KEY_W, 1.f );
		m_Forward->AddKeyboardBinding( GLFW_KEY_S, -1.f );

		m_Sideways = Pine::Input->CreateBinding( "Sideways" );
		m_Sideways->AddKeyboardBinding( GLFW_KEY_D, 1.f );
		m_Sideways->AddKeyboardBinding( GLFW_KEY_A, -1.f );
	}

	void OnRender( ) override
	{
		if ( !Globals::IsInLevelView )
			return;

		if ( Globals::IsHoveringLevelView )
		{
			if ( ImGui::IsMouseDown( ImGuiMouseButton_::ImGuiMouseButton_Right ) )
			{
				m_IsMovingCamera = true;
			}
		}

		if ( m_IsMovingCamera )
		{
			if ( !ImGui::IsMouseDown( ImGuiMouseButton_::ImGuiMouseButton_Right ) )
				m_IsMovingCamera = false;

			const auto pitch = m_Pitch->Value( );
			const auto yaw = m_Yaw->Value( );

			GetParent( )->GetTransform( )->Rotation += glm::vec3( pitch, yaw, 0.f );

			const auto forwardMove = m_Forward->Value( );
			const auto sideMove = m_Sideways->Value( );

			GetParent( )->GetTransform( )->Position += GetParent( )->GetTransform( )->GetForward( ) * .15f * forwardMove;
			GetParent( )->GetTransform( )->Position += GetParent( )->GetTransform( )->GetRight( ) * .15f * sideMove;
		}
	}

	void OnUpdate( float deltaTime ) override
	{
	}

};

void Editor::EditorEntity::Create( )
{
	g_EditorEntity = Pine::EntityList->CreateEntity( );

	g_EditorEntity->SetTemporary( true );
	g_EditorEntity->SetName( "Editor Entity" );

	g_EditorEntity->AddComponent( Pine::EComponentType::Camera );
	g_EditorEntity->RegisterComponent( new EditorEntityScript( ) ); // this sucks.

	// We have to manually call OnCreated for these soon to be unsupported NativeScript
	g_EditorEntity->GetComponent<EditorEntityScript>( )->OnCreated( );

	g_Camera = g_EditorEntity->GetComponent<Pine::Camera>( );
}

Pine::Entity* Editor::EditorEntity::GetEntity( )
{
	return g_EditorEntity;
}

Pine::Camera* Editor::EditorEntity::GetCamera( )
{
	return g_Camera;
}
