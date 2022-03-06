#pragma once
#include "../IAsset/IAsset.hpp"

namespace Pine
{
	class Texture2D;

	enum class Texture3DType
	{
		SingleTexture,
		MultiTexture
	};

	enum Texture3DSide
	{
		Right,
		Left,
		Top,
		Bottom,
		Front,
		Back
	};

	class Texture3D : public IAsset
	{
	private:
		unsigned int m_Id = 0;

		// Mutli texture
		Pine::Texture2D* m_MultiTextures[ 6 ] = { nullptr };

		Texture3DType m_Texture3DType = Texture3DType::MultiTexture;

		void Rebuild( );
	public:
		Texture3D( );

		unsigned int GetId( ) const;

		void SetTexture3DType( Texture3DType type );
		Texture3DType GetTexture3DType( ) const;

		void SetMultiTexture( Texture3DSide side, Texture2D* texture );
		Pine::Texture2D* GetMultiTexture( Texture3DSide side ) const;

		void Bind( ) const;

		bool LoadFromFile( ) override;
		bool SaveToFile( ) override;
		void Dispose( ) override;

	};

}