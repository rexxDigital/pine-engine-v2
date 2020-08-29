#pragma once
#include "../IAsset/IAsset.hpp"

namespace Pine {

	class Texture3D : public IAsset {
	private:
		unsigned int m_Id = 0;
	public:
		Texture3D();

		unsigned int GetId() const;

		void Bind() const;

		bool LoadFromFile() override;
		bool SaveToFile() override;
		void Dispose() override;

	};

}