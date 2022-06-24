#include "EditorEntity.hpp"

#include <GLFW/glfw3.h>
#include <Pine/Components/NativeScript/NativeScript.hpp>

#include "Pine/Components/Camera/Camera.hpp"
#include "Pine/Entitylist/EntityList.hpp"
#include "Pine/Input/Input.hpp"

#include "../Gui/Gui.hpp"
#include "ImGui/imgui.h"
#include "Pine/Components/Components.hpp"

namespace
{
	Pine::Entity* g_EditorEntity = nullptr;
	Pine::Camera* g_Camera = nullptr;
}

using namespace Editor::Gui;

class EditorEntityScript : public Pine::IComponent
{
private:
	bool m_IsMovingCamera = false;
public:

	void OnSetup( ) override
	{
	}

	void OnCreated( ) override
	{
	}

	void OnRender( ) override
	{
		if ( !Globals::IsInLevelView )
			return;

		if ( Globals::IsHoveringLevelView )
		{
			if ( ImGui::IsMouseDown( ImGuiMouseButton_Right ) )
			{
				m_IsMovingCamera = true;
			}
		}

		if ( m_IsMovingCamera )
		{
			if ( !ImGui::IsMouseDown( ImGuiMouseButton_Right ) )
				m_IsMovingCamera = false;

            const auto mouseDelta = Pine::Input->GetMouseDelta( );

			const auto pitch = mouseDelta.y * 0.15f;
			const auto yaw = mouseDelta.x * 0.15f;

			GetParent( )->GetTransform( )->Rotation += glm::vec3( pitch, yaw, 0.f );

            auto forwardMove = 0.f;
            auto sideMove = 0.f;

            if ( Pine::Input->IsKeyDown( GLFW_KEY_W ) )
                forwardMove += 1.f;
            if ( Pine::Input->IsKeyDown( GLFW_KEY_S ) )
                forwardMove -= 1.f;
            if ( Pine::Input->IsKeyDown( GLFW_KEY_D ) )
                sideMove += 1.f;
            if ( Pine::Input->IsKeyDown( GLFW_KEY_A ) )
                sideMove -= 1.f;


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

	g_EditorEntity->AddComponent( Pine::ComponentType::Camera );
	g_EditorEntity->AddScript( new EditorEntityScript );

	g_Camera = g_EditorEntity->GetComponent<Pine::Camera>( );

	g_Camera->SetFarPlane( 2000.f );
}

Pine::Entity* Editor::EditorEntity::GetEntity( )
{
	return g_EditorEntity;
}

Pine::Camera* Editor::EditorEntity::GetCamera( )
{
	return g_Camera;
}
