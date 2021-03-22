#include "Timer.hpp"

Pine::Timer::Timer( )
{
	m_BeginTime = std::chrono::high_resolution_clock::now( );
}

void Pine::Timer::Stop( )
{
	m_EndTime = std::chrono::high_resolution_clock::now( );
}

float Pine::Timer::GetElapsedTimeInMs( ) const
{
	const std::chrono::duration<float, std::milli> msFloat = m_EndTime - m_BeginTime;

	return msFloat.count(  );
}
