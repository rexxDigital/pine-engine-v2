#include "IAsset.hpp"

#include "../../Core/Log/Log.hpp"

void Pine::IAsset::SetFilePath(const std::string& str)
{
	m_FilePath = std::filesystem::path(str);
	m_FileName = m_FilePath.filename().string();
	m_AbsoluteFilePath = std::filesystem::absolute( m_FilePath ).string( );
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
	return m_AbsoluteFilePath;
}

Pine::EAssetType Pine::IAsset::GetType() const
{
	return m_Type;
}

void Pine::IAsset::UpdateLastWriteTime( ) {
	try
	{
		// Apparently only this could break.
		m_LastWriteTime = std::filesystem::last_write_time( m_AbsoluteFilePath ).time_since_epoch( );
	}
	catch ( std::exception& e )
	{
		Log::Warning( "Failed to set last write time on asset, " + m_FilePath.string( ) );
		Log::Warning( e.what( ) );
	}
}

bool Pine::IAsset::HasBeenUpdated( ) const {
	const auto currentWriteTime = std::filesystem::last_write_time( m_FilePath );
	return currentWriteTime.time_since_epoch( ) != m_LastWriteTime;
}

bool Pine::IAsset::GetReadOnly( ) const {
	return m_ReadOnly;
}

void Pine::IAsset::SetReadOnly( bool value ) {
	m_ReadOnly = value;
}

bool Pine::IAsset::GetUpdated( ) const
{
	return m_Updated;
}

void Pine::IAsset::SetUpdated( bool value )
{
	m_Updated = value;
}

bool Pine::IAsset::IsMapped( ) const
{
	return m_IsMapped;
}

void Pine::IAsset::SetMapped( bool value )
{
	m_IsMapped = value;
}
