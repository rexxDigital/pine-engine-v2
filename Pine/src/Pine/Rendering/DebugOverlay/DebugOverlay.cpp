#include "DebugOverlay.hpp"

#include <vector>
#include <glm/vec2.hpp>

#include "../../../ImGui/imgui.h"

namespace Pine
{
	// All of this could be done in a nicer cleaner implementation but this will do for debugging.

	struct RectDraw
	{
		glm::vec4 m_PosSize;
		bool m_Filled;
		glm::vec4 m_Color;
	};

	struct TextDraw
	{
		glm::vec2 m_Position;
		std::string m_Text;
		glm::vec4 m_Color;
	};

	class CDebugOverlay : public IDebugOverlay
	{
	private:
		glm::vec4 m_Viewport;

		std::vector<RectDraw> m_RectangleDrawCalls;
		std::vector<TextDraw> m_TextDrawCalls;

	public:

		void SetViewport( int x, int y, int w, int h ) override
		{
			m_Viewport.x = x;
			m_Viewport.y = y;
			m_Viewport.z = w;
			m_Viewport.w = h;
		}

		glm::vec4 GetViewport( ) override
		{
			return m_Viewport;
		}

		void RenderRectangle( int x, int y, int w, int h, glm::vec4 color ) override
		{
			m_RectangleDrawCalls.push_back( { glm::vec4( x, y, w, h ), false, color } );
		}

		void RenderFilledRectangle( int x, int y, int w, int h, glm::vec4 color ) override
		{
			m_RectangleDrawCalls.push_back( { glm::vec4( x, y, w, h ), true, color } );
		}

		void RenderText( int x, int y, const std::string& text, glm::vec4 color ) override
		{
			m_TextDrawCalls.push_back( { glm::vec2( x, y ), text, color } );
		}

		void Render( ) override
		{
			for ( const auto& rect : m_RectangleDrawCalls )
			{
				if ( rect.m_Filled )
					ImGui::GetForegroundDrawList( )->AddRectFilled( ImVec2( m_Viewport.x + rect.m_PosSize.x, m_Viewport.y + rect.m_PosSize.y ), ImVec2( m_Viewport.x + rect.m_PosSize.x + rect.m_PosSize.z, m_Viewport.y + rect.m_PosSize.y + rect.m_PosSize.w ), ImColor( rect.m_Color.x, rect.m_Color.y, rect.m_Color.z, rect.m_Color.w ) );
				else
					ImGui::GetForegroundDrawList( )->AddRect( ImVec2( m_Viewport.x + rect.m_PosSize.x, m_Viewport.y + rect.m_PosSize.y ), ImVec2( m_Viewport.x + rect.m_PosSize.x + rect.m_PosSize.z, m_Viewport.y + rect.m_PosSize.y + rect.m_PosSize.w ), ImColor( rect.m_Color.x, rect.m_Color.y, rect.m_Color.z, rect.m_Color.w ) );
			}

			for ( const auto& text : m_TextDrawCalls )
			{
				ImGui::GetForegroundDrawList( )->AddText( ImVec2( m_Viewport.x + text.m_Position.x, m_Viewport.y + text.m_Position.y ), ImColor( text.m_Color.x, text.m_Color.y, text.m_Color.z, text.m_Color.w ), text.m_Text.c_str( ) );
			}

			m_RectangleDrawCalls.clear( );
			m_TextDrawCalls.clear( );
		}

	};

	IDebugOverlay* CreateDebugOverlay( )
	{
		return new CDebugOverlay;
	}

}