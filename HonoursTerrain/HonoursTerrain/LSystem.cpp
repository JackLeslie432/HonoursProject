#include "LSystem.h"

LSystem::LSystem(string Axiom) :
	m_Axiom(Axiom)
{
	m_CurrentSystem = Axiom;
}

void LSystem::Run(const int count)
{
	m_CurrentSystem = m_Axiom;
	for (int i = 0; i < count; i++)
	{
		Iterate();
	}
}

void LSystem::AddRule(char predecessor, string successor)
{
	Rule temp;

	temp.predecessor = predecessor;
	temp.successor = successor;

	baseRules.push_back(temp);
}

void LSystem::AddStochRule(const char predecessor, std::vector<std::pair<string, float>> successor)
{
	StochRule temp;

	temp.predecessor = predecessor;
	temp.successors = successor;

	for (int i = 0; i < successor.size(); i++)
	{
		temp.totalProbability += successor[i].second;
	}

	stochRules.push_back(temp);
}

void LSystem::AddContextRule(const char predecessor, const char prev, const char following, const string successor)
{
	ContextRule temp;

	temp.predecessor = predecessor;
	temp.prev = prev;
	temp.following = following;
	temp.successor = successor;

	contextRules.push_back(temp);
}

//Evolve the system one time
void LSystem::Iterate()
{
	string start = m_CurrentSystem;
	int count = 0;

	for (auto s : start)
	{
		bool rule = false;
		if (useStochRules)
		{
			for (int i = 0; i < stochRules.size(); i++)
			{
				if (s == stochRules[i].predecessor)
				{
					int random = rand() % stochRules[i].totalProbability;

					int j = 0;

					while (j < stochRules[i].successors.size())
					{
						random -= stochRules[i].successors[j].second;
						if (random < 0)
						{
							break;
						}
						j++;
					}

					if (random < stochRules[i].successors[j].second)
					{
						m_CurrentSystem.replace(count, 1, stochRules[i].successors[j].first);
						count += stochRules[i].successors[j].first.size();
						rule = true;
						break;
					}
				}
			}
		}

		if (useContextRules)
		{
			for (int i = 0; i < contextRules.size(); i++)
			{
				if (s == contextRules[i].predecessor)
				{
					if (m_CurrentSystem[count - 1] == contextRules[i].prev && count + 1 > m_CurrentSystem.size() && m_CurrentSystem[count + 1] == contextRules[i].following)
					{
						m_CurrentSystem.replace(count, 1, contextRules[i].successor);
						count += contextRules[i].successor.size();
						rule = true;
						break;
					}
				}
			}
		}

		if (!rule)
		{
			for (int i = 0; i < baseRules.size(); i++)
			{
				if (s == baseRules[i].predecessor)
				{
					m_CurrentSystem.replace(count, 1, baseRules[i].successor);
					count += baseRules[i].successor.size();
					rule = true;
					break;
				}
			}
		}

		// increment by 1 if not rule found
		if (!rule)
			count++;
	}
}