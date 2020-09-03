#include "IAsset.hpp"
#include "../../OpenGL/FrameBuffer/FrameBuffer.hpp"

void Pine::IAsset::SetFilePath(const std::string& str)
{
	m_FilePath = std::filesystem::path(str);
	m_FileName = m_FilePath.filename().string();
}

const std::filesystem::path& Pine::IAsset::GetPath() const
{
	return m_FilePath;
}

const std::string& Pine::IAsset::GetFileName() const
{
	return m_FileName;
}

Pine::EAssetType Pine::IAsset::GetType() const
{
	return m_Type;
}
