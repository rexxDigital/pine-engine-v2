#pragma once
#include "../Assets.hpp"
#include "../../OpenGL/ShaderProgram/ShaderProgram.hpp"

namespace Pine {

	class Shader : public IAsset, public ShaderProgram {
	private:
	public:
		Shader();

		bool LoadFromFile() override;
		bool SaveToFile() override;
		void Dispose() override;
	};

}