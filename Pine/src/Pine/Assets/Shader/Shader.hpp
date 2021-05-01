#pragma once
#include "../Assets.hpp"
#include "../../OpenGL/ShaderProgram/ShaderProgram.hpp"

namespace Pine {

	class Shader : public IAsset, public ShaderProgram {
	private:
		std::vector<IAsset*> m_AttachedShaderFiles;
	public:
		Shader( );

		const std::vector<IAsset*> GetAttachedShaderFiles( ) const;
		
		bool LoadFromFile( ) override;
		bool SaveToFile( ) override;
		void Dispose( ) override;
	};

}