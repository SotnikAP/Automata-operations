#include <iostream>
#include <fstream>
#include <string>
#include "Automaton.h"

void error(const std::string& e)
{
	std::cerr << e << std::endl;
}

int main(int argc, char* argv[])
{
	if (argc > 3)// если передаем аргументы, то argc будет больше 1(в зависимости от кол-ва аргументов)
	{
		error("Too much arguments.");
		return 1;
	}
	else if (argc < 3)
	{
		error("Execute with two files in .fsm format.");
		return 2;
	}

	std::ifstream leftAutomFile; //Открываем файл
	std::ifstream rightAutomFile;
	std::ofstream rightQFile, leftQFile;

	leftAutomFile.open(argv[1]);
	rightAutomFile.open(argv[2]);
	rightQFile.open("rightQuotient.txt");
	leftQFile.open("leftQuotient.txt");

	Automaton leftAutomaton;
	Automaton rightAutomaton;

	leftAutomFile >> leftAutomaton;
	rightAutomFile >> rightAutomaton;

	Automaton rightQuotient = leftAutomaton.rightQuotient(rightAutomaton);
	Automaton leftQuotient = leftAutomaton.leftQuotient(rightAutomaton);

	rightQFile << rightQuotient;
	leftQFile << leftQuotient;

	leftAutomFile.close();
	rightAutomFile.close();
	rightQFile.close();
	leftQFile.close();
	std::cout << "Successful!" << std::endl;
	//system("pause");
}