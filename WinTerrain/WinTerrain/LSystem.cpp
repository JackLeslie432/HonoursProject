#include "LSystem.h"
#include <time.h>

LSystem::LSystem(string Axiom) :
	m_Axiom(Axiom)
{
	m_CurrentSystem = Axiom;

	srand(time(NULL));
}

// Run the system for given iterations
void LSystem::Run(const int count, bool reset)
{
	if (reset)
		m_CurrentSystem = m_Axiom;

	for (int i = 0; i < count; i++)	
		Iterate();
}

// Add a normal rule
void LSystem::AddRule(char predecessor, string successor)
{
	Rule temp;

	temp.predecessor = predecessor;
	temp.successor = successor;

	mapRule[predecessor] = temp;
}

// Add a stochastic rule (Has probability to change to many values)
void LSystem::AddStochRule(const char predecessor, std::vector<std::pair<string, float>> successor)
{
	StochRule temp;

	temp.predecessor = predecessor;
	temp.successors = successor;

	// add up all he probabilities and add to the total
	for (auto & i : successor)
	{
		temp.totalProbability += i.second;
	}

	mapStochRule[predecessor] = temp;
}

// Add a context rule rule (Changes depending on surrounding values)
void LSystem::AddContextRule(const char predecessor, const char prev, const char following, const string successor)
{
	// Create temp to then add to rule list
	ContextRule temp;

	// Add passed in values then push to list of all rules
	temp.predecessor = predecessor;
	temp.prev = prev;
	temp.following = following;
	temp.successor = successor;

	mapContextRule[predecessor] = temp;
}

void LSystem::RemoveRule(const char predecessor)
{
	mapRule.erase(predecessor);
}

void LSystem::RemoveStochRule(const char predecessor)
{
	mapStochRule.erase(predecessor);
}

void LSystem::RemoveContextRule(const char predecessor)
{
	mapContextRule.erase(predecessor);
}

//Evolve the system one time
void LSystem::Iterate()
{
	// Store what the system is at the start
	string start = m_CurrentSystem;
	int count = 0;

	// Loop through the whole system
	for (auto s : start)
	{
		bool rule = false;

		// Iterate through stochastic rules
		rule = IterateStochRule(s, rule, count);

		// Iterate though all context rules
		rule = IterateContextRule(s, rule, count);

		// iterate through base rules
		rule = IterateRule(s, rule, count);

		// increment by 1 if no rule found
		if (!rule)
			count++;
	}
}

bool LSystem::IterateRule(char s, bool rule, int& count)
{
	if (!rule)
	{
		for (const auto& rules : mapRule)
		{
			if (s == rules.first)
			{
				m_CurrentSystem.replace(count, 1, rules.second.successor);
				count += rules.second.successor.size();
				return true;
			}
		}
	}
	return false;
}

bool LSystem::IterateStochRule(char s, bool rule, int& count)
{
	if (useStochRules)
	{
		for (const auto& rules : mapStochRule)
		{
			if (s == rules.first)
			{
				// Find a random number thats within the bounds of the probability
				int random = rand() % rules.second.totalProbability;

				int j = 0;

				// Loop through all the possible successors
				while (j < rules.second.successors.size())
				{
					// Remove the probability of the current successor
					random -= rules.second.successors[j].second;
					if (random < 0) // Stop when reached 0
						break;
						
					j++;
				}

				// replace the current letter with the rule
				if (random < rules.second.successors[j].second)
				{
					if (rules.second.successors[j].second == rules.first)
						break;

					m_CurrentSystem.replace(count, 1, rules.second.successors[j].first);
					count += rules.second.successors[j].first.size();
					return true;
				}
			}
		}
	}

	return false;
}

bool LSystem::IterateContextRule(char s, bool rule, int& count)
{
	if (useContextRules && !rule)
	{
		for (const auto& rules : mapContextRule)
		{
			if (s == rules.first)
			{
				if (m_CurrentSystem[count - 1] == rules.second.prev && count + 1 < m_CurrentSystem.size() && m_CurrentSystem[count + 1] == rules.second.following)
				{
					m_CurrentSystem.replace(count, 1, rules.second.successor);
					count += rules.second.successor.size();
					return true;
				}
			}
		}
	}
	return false;
}
