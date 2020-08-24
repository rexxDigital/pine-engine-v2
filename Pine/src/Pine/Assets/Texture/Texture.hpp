#pragma once
#include "../IAsset/IAsset.hpp"

namespace Pine {

	class Texture : public IAsset {
	private:
		unsigned int m_Id = 0;
		int m_Width = 0;
		int m_Height = 0;
	public:
		Texture();

		unsigned int GetId() const;
		int GetWidth() const;
		int GetHeight() const;

		bool LoadFromFile() override;
		bool SaveToFile() override;
		void Dispose() override;
	};

}