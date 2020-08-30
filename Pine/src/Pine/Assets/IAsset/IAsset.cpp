#include "IAsset.hpp"
#include "../../OpenGL/FrameBuffer/FrameBuffer.hpp"

void Pine::IAsset::SetFilePath(const std::string& str)
{
	m_FilePath = std::filesystem::path(str);
}

const std::filesystem::path& Pine::IAsset::GetPath() const
{
	return m_FilePath;
}

Pine::EAssetType Pine::IAsset::GetType() const
{
	return m_Type;
}

bool Pine::IAsset::HasAvailablePreview() const {
	return m_PreviewFrameBuffer != nullptr;
}

const int Pine::IAsset::GetAssetPreview() const {
	if (m_PreviewFrameBuffer) {
		return m_PreviewFrameBuffer->GetTextureId();
	}

	return -1;
}

void Pine::IAsset::GenerateAssetPreview() {

}
