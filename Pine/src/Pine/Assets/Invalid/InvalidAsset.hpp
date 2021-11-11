#pragma once
#include "../IAsset/IAsset.hpp"

namespace Pine
{

	class InvalidAsset : public IAsset
	{
	private:
	public:

		bool SaveToFile( ) override;
		bool LoadFromFile( ) override;
		void Dispose( ) override;

	};

}
