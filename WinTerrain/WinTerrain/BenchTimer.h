#pragma once
#include <iostream>
#include <chrono>
#include <ctime>
#include <cmath>

class BenchTimer
{
public:

	float GetTime();

	void Start();
	void End();

private:

	float elapsedTime;
	bool m_running = false;

	std::chrono::time_point<std::chrono::system_clock> m_startTime;
};

