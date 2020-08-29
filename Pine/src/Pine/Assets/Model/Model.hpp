#pragma once
#include "../IAsset/IAsset.hpp"
#include "../Mesh/Mesh.hpp"

namespace Pine {

	class Model : public IAsset {
	private:
		std::vector<Pine::Mesh*> m_MeshList;
	public:
		Model();

		Pine::Mesh* CreateMesh();

		const std::vector<Pine::Mesh*>& GetMeshList() const;

		bool LoadFromFile() override;
		bool SaveToFile() override;
		void Dispose() override;
		void GenerateAssetPreview() override;

	};

}