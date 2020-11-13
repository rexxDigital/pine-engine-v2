#include "IAsset.hpp"

void Pine::IAsset::SetFilePath(const std::string& str)
{
	m_FilePath = std::filesystem::path(str);
	m_FileName = m_FilePath.filename().string();
	m_AbsoulteFilePath = std::filesystem::absolute( m_FilePath ).string( );
}

const std::filesystem::path& Pine::IAsset::GetPath() const
{
	return m_FilePath;
}

const std::string& Pine::IAsset::GetFileName() const
{
	return m_FileName;
}

const std::string& Pine::IAsset::GetAbsoluteFilePath( ) const {
	return m_AbsoulteFilePath;
}

Pine::EAssetType Pine::IAsset::GetType() const
{
	return m_Type;
}

void Pine::IAsset::UpdateLastWriteTime( ) {
	m_LastWriteTime = std::filesystem::last_write_time( m_FilePath ).time_since_epoch( );
}

bool Pine::IAsset::HasBeenUpdated( ) const {
	const auto currentWriteTime = std::filesystem::last_write_time( m_FilePath );
	return currentWriteTime.time_since_epoch( ) != m_LastWriteTime;
}
