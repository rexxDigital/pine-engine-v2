#include "Math.hpp"

#include <glm/ext/matrix_projection.hpp>

#include "../../Rendering/DebugOverlay/DebugOverlay.hpp"
#include "../../Rendering/RenderManager/RenderManager.hpp"

namespace Pine
{

	class CMath : public IMath
	{
	private:
	public:

		glm::vec3 WorldToScreen( const glm::vec3& inputPosition, Camera* cam ) override
		{
			// TODO: Update this to something else.
			const auto viewport = DebugOverlay->GetViewport( );

			auto screenPos = 
				glm::project( 
				inputPosition,
				cam->GetViewMatrix( ), 
				cam->GetProjectionMatrix( ), 
				glm::vec4( 0.f, 0.f, viewport.z, viewport.w ) );

			// Since the output will always be rendered up side down, we'll also have to flip the
			// Y coordinate so it's correct on the final output.
			screenPos.y = viewport.w - screenPos.y;

			return screenPos;
		}

		glm::vec3 WorldToScreen( const glm::vec3& inputPosition ) override
		{
			return WorldToScreen( inputPosition, RenderManager->GetRenderingContext( )->m_Camera );
		}

	};

	IMath* CreateMath( )
	{
		return new CMath;
	}

}
