//////////////////////////////////////
//05.12.17 - right quotient release
//25.02.18 - left quotient development begin 
//////////////////////////////////////
#pragma once
#include <iostream>

/////////////////////////////////////
//Gets two automata in .fsm format as input
//Able to build left or right quotient
//File or console output for automata
//Automata for LQ or RQ have to be determinate, fully defind
/////////////////////////////////////

struct Transition
{
	bool fin_m;
	bool init_m;
	int* Actions_m;
	Transition();
	~Transition();
};

//Automaton structure
class Automaton
{
	int numOfStates_m;
	int numOfActions_m;
	Transition* transitions_m;
public:
	//constructors
	Automaton();
	Automaton(const Automaton& initAutom);   //copy constructor
											 //destructor
	~Automaton();
	//methods
	Automaton rightQuotient(const Automaton& secAutomaton) const;
	Automaton leftQuotient(const Automaton& secAutomaton) const;
	//operators
	Automaton& operator=(const Automaton& v);
	//friends
	friend bool intersecForRightQuot(const Automaton& leftAutom, const Automaton& rightAutom, int leftInitState);   //1 if entersection is not empty
	friend int intersecForLeftQuot(const Automaton& leftAutom, const Automaton& rightAutom, int leftInitState = 0);	//return state, which will be init in left automaton
	friend std::ostream& operator<<(std::ostream& os, const Automaton& automaton);
	friend std::ofstream& operator<<(std::ofstream& ofs, const Automaton& automaton);
	friend std::ifstream& operator >> (std::ifstream& ifs, Automaton& automaton);
};