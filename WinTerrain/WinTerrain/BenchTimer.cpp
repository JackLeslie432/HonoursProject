#include "BenchTimer.h"

float BenchTimer::GetTime()
{
	std::chrono::time_point<std::chrono::system_clock> endTime;

	if (m_running)
	{
		endTime = std::chrono::system_clock::now();
		elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_startTime).count();
	}

	return elapsedTime;
}

void BenchTimer::Start()
{
	m_startTime = std::chrono::system_clock::now();
	m_running = true;
}

void BenchTimer::End()
{
	m_startTime = std::chrono::system_clock::now();
	m_running = false;
}