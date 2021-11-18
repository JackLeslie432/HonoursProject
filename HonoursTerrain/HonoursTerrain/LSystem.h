#pragma once
#include <iostream>
#include <vector>

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
		// Vector of all the rules and their probabilty
		std::vector<std::pair<string, float>>successors;
	};

	// Context senstative
	struct ContextRule
	{
		char predecessor;

		char prev;
		char following;

		string successor;
	};

	// Parametric
	struct ParametricRule
	{
		char predecessor;
	};

	// Functions

	LSystem(string Axiom);

	inline void SetAxiom(string Axiom) { m_Axiom = Axiom; }
	inline void SetUsedRules(bool stoch, bool context) { useStochRules = stoch; useContextRules = context; }

	//Get the string that represents the current state of the L-System
	inline const string GetCurrentSystem() { return m_CurrentSystem; }
	inline const string GetAxiom() { return m_Axiom; }

	void AddRule(const char, const string);			//Add a default rule to the system
	void AddStochRule(const char, std::vector<std::pair<string, float>>);	//Add a Stochastic rule to the system
	void AddContextRule(const char, const char, const char, const string);	//Add a Context rule to the system
	void Run(const int count);						//Iterate the system a set number of times
	void Iterate();									//Apply the rules one time

private:

	string m_Axiom;
	string m_CurrentSystem;

	std::vector<Rule> baseRules;
	std::vector<StochRule> stochRules;
	std::vector<ContextRule> contextRules;

	bool useStochRules = true;
	bool useContextRules = true;
};