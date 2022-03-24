#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>

using std::string;

class LSystem
{
public:
	// Normal Rule
	struct Rule
	{
		char predecessor;
		string successor;
	};

	// Stochastic (probability)
	struct StochRule
	{
		char predecessor;
		int totalProbability = 0;
		// Vector of all the rules and their probability
		std::vector<std::pair<string, float>>successors;
	};

	// Context sensitive
	struct ContextRule
	{
		char predecessor;

		char prev;
		char following;

		string successor;
	};

	// Functions

	LSystem(string Axiom);

	inline void SetAxiom(string Axiom) { m_Axiom = Axiom; }
	inline void SetUsedRules(bool stoch, bool context) { useStochRules = stoch; useContextRules = context; }

	//Get the string that represents the current state of the L-System
	inline const string GetCurrentSystem() { return m_CurrentSystem; }
	inline const string GetAxiom() { return m_Axiom; }

	void AddRule(const char predecessor, const string successor);			//Add a default rule to the system
	void AddStochRule(const char predecessor, std::vector<std::pair<string, float>> successor);	//Add a Stochastic rule to the system
	void AddContextRule(const char predecessor, const char prev, const char following, const string successor);	//Add a Context rule to the system

	void RemoveRule(const char predecessor);
	void RemoveStochRule(const char predecessor);
	void RemoveContextRule(const char predecessor);

	void ClearRule() { mapRule.clear();};
	void ClearStochRule() { mapStochRule.clear(); };
	void ClearContextRule() { mapContextRule.clear(); };

	std::unordered_map<char, Rule> GetRules() { return mapRule;};
	std::unordered_map<char, StochRule> GetStochRules() { return mapStochRule; };
	std::unordered_map<char, ContextRule> GetContextRules() { return mapContextRule; };

	void Run(const int count, bool reset);						//Iterate the system a set number of times
	void Iterate();									//Apply the rules one time

private:

	string m_Axiom;
	string m_CurrentSystem;

	std::unordered_map<char, Rule> mapRule;
	std::unordered_map<char, StochRule> mapStochRule;
	std::unordered_map<char, ContextRule> mapContextRule;

	bool useStochRules = true;
	bool useContextRules = true;

	bool IterateRule(char s, bool rule, int& count);
	bool IterateStochRule(char s, bool rule, int& count);
	bool IterateContextRule(char s, bool rule, int& count);

};