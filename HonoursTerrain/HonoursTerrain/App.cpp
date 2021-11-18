#include <iostream>
#include "LSystem.h"
#include <time.h>
#include "App.h"

int main()
{
	srand(time(NULL));

	LSystem system("a");
	system.SetUsedRules(true, true);

	// Adding normal rules
	system.AddRule('a', "c");

	// Adding socastic rules
	std::vector<std::pair<string, float>> rules;
	std::pair<string, float> temp;

	temp.first = "a";
	temp.second = 50;

	rules.push_back(temp);

	temp.first = "b";
	temp.second = 50;

	rules.push_back(temp);

	system.AddStochRule('a', rules);

	// Adding contex rules
	system.AddContextRule('a', 'a', 'b', "c");

	system.Iterate();

	std::cout << "Axiom: " << system.GetAxiom() << std::endl;
	std::cout << "Current system: " << system.GetCurrentSystem() << std::endl;

	std::system("pause");

	return 0;
}

bool App::Frame()
{
	// Render the graphics.
	bool result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App::Render()
{
	return true;
}
