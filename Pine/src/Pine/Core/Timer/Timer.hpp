#pragma once
#include <chrono>

namespace Pine
{

	class Timer
	{
	private:
		std::chrono::system_clock::time_point m_BeginTime;
		std::chrono::system_clock::time_point m_EndTime;
	public:
		Timer( );

		void Stop( );
		
		double GetElapsedTimeInMs( ) const;
	};
	
}