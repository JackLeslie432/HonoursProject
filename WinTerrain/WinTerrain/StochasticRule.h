#pragma once
#include <vector>
#include <string>

// Class to manage many rules and probabilities for stochastic rules
class StochasticRule
{
public:

	//StochasticRule();
	//~StochasticRule();

	// Add a new outcome
	void Add(std::string successor, int probability);

	//Return
	std::vector<std::pair<std::string, float>> GetRules() { return rules; };

protected:

	std::vector<std::pair<std::string, float>> rules;
};

