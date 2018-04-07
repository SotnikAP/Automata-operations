/// @file
/// @brief Main programm.
/// @author Sotnikov A.P.
/// @date 07.04.2018
///
/// <pre>
/// Reads two automata from files and makes left and right quotions.
/// GitHub: https://github.com/SotnikAP/Automata-operations
/// </pre>

#include <iostream>
#include <fstream>
#include <string>

#include "Automaton.h"

void error(const std::string& e)
{
	std::cerr << e << std::endl;
}

int main( int argc, char* argv[] )
{
     if ( argc > 3 )// если передаем аргументы, то argc будет больше 1(в зависимости от кол-ва аргументов)
     {
          error( "Too much arguments." );
          return 1;
     }
     else if ( argc < 3 )
     {
          error( "Execute with two files in .fsm format." );
          return 2;
     }

     std::ifstream leftAutomFile( argv[1] ); //Открываем файл
     std::ifstream rightAutomFile( argv[2] );
     std::ofstream rightQFile, leftQFile;

     rightQFile.open( "rightQuotient.txt" );
     leftQFile.open( "leftQuotient.txt" );

     automaton::Automaton leftAutomaton;
     automaton::Automaton rightAutomaton;

     leftAutomFile >> leftAutomaton;
     rightAutomFile >> rightAutomaton;

     automaton::Automaton rightQuotient = leftAutomaton.rightQuotient( rightAutomaton );
     automaton::Automaton leftQuotient = leftAutomaton.leftQuotient( rightAutomaton );

     rightQFile << rightQuotient;
     leftQFile << leftQuotient;

     leftAutomFile.close();
     rightAutomFile.close();
     rightQFile.close();
     leftQFile.close();
     std::cout << "Successful!" << std::endl;
     system( "pause" );
}