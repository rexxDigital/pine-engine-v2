#pragma once
#include <chrono>

namespace Pine
{

	class Timer
	{
	private:
		std::chrono::steady_clock::time_point m_BeginTime;
		std::chrono::steady_clock::time_point m_EndTime;
	public:
		Timer( );

		void Stop( );
		
		double GetElapsedTimeInMs( ) const;
	};
	
}