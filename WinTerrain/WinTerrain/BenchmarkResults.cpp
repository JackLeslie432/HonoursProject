#include "BenchmarkResults.h"

BenchmarkResults::BenchmarkResults(int iteration, int res, std::string testName)
{
	m_iterations = iteration;
	m_resolution = res;
	m_testName = testName;
}

void BenchmarkResults::StartTime()
{
	m_timer.Start();
}

void BenchmarkResults::StopTime()
{
	m_timer.End();
}

void BenchmarkResults::NextIteration()
{
	m_times.push_back(m_timer.GetTime());
	m_timer.Start();
}

void BenchmarkResults::WriteToFile()
{
	outputFile.open(m_testName + ".csv");

	outputFile << m_testName << " at resolution " << m_resolution <<"\n";
	int count = 1;

	for (auto time : m_times)
	{

		outputFile << count << ",";
		outputFile << time << "\n";

		count++;
	}

	outputFile.close();
}
