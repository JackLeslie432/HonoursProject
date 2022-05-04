#pragma once

#include "BenchTimer.h"
#include <list>
#include <fstream>

class BenchmarkResults
{
public:

	BenchmarkResults(int iteration, int res, std::string testName);
	~BenchmarkResults() = default;

	int IterationAmount() { return m_iterations; };

	void StartTime();
	void StopTime();
	void NextIteration();

	void WriteToFile();

private:

	std::ofstream outputFile;

	std::string m_testName;
	BenchTimer m_timer;
	int m_resolution;
	int m_iterations;

	std::list<float> m_times;
};

