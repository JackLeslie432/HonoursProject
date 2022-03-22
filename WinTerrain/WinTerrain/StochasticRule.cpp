#include "StochasticRule.h"

// Add a rule to the array of rules
void StochasticRule::Add(std::string successor, int probability)
{
	std::pair<std::string, float> temp;

	temp.first = successor;
	temp.second = probability;

	rules.push_back(temp);
}
