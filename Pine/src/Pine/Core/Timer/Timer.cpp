#include "Timer.hpp"

Pine::Timer::Timer( )
{
	m_BeginTime = std::chrono::high_resolution_clock::now( );
}

void Pine::Timer::Stop( )
{
	m_EndTime = std::chrono::high_resolution_clock::now( );
}

double Pine::Timer::GetElapsedTimeInMs( ) const
{
	const std::chrono::duration<double, std::milli> msFloat = m_EndTime - m_BeginTime;

	return msFloat.count(  );
}
