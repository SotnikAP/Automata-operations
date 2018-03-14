//////////////////////////////////////////////
//		Sotnikov A.P. diploma project		//
//			Operations on automata			//
//////////////////////////////////////////////

#include <list>
#include <algorithm>
#include <fstream>
#include <cstdlib>   //for atoi
#include "Automaton.h"


//Transition on it's own is useless
//Array of transitions is automaton
//State of automaton is array index
Transition::Transition()
{
	fin_m = 0;
	init_m = 0;
	Actions_m = nullptr;	//array, where index is ACTION and the elements are states automaton will reach from current state by current ACTION (index) 
}
//destructor
Transition::~Transition()
{
	delete[] Actions_m;
}
//Transition end//

//Automaton class//
//default constructor
Automaton::Automaton()
{
	numOfStates_m = 0;
	numOfActions_m = 0;
	transitions_m = nullptr;
}

//copy constructor
Automaton::Automaton(const Automaton & initAutom)
{
	numOfActions_m = initAutom.numOfActions_m;
	numOfStates_m = initAutom.numOfStates_m;
	transitions_m = new Transition[numOfStates_m];   //memory allocation for array of transitions
	for (int i = 0; i < numOfStates_m; i++)   	 //deep copying
	{
		transitions_m[i].fin_m = initAutom.transitions_m[i].fin_m;
		transitions_m[i].init_m = initAutom.transitions_m[i].init_m;
		transitions_m[i].Actions_m = new int[numOfActions_m];
		for (int j = 0; j < numOfActions_m; j++)
			transitions_m[i].Actions_m[j] = initAutom.transitions_m[i].Actions_m[j];
	}
}

//destructor
Automaton::~Automaton()
{
	delete[]transitions_m;
}

Automaton& Automaton::operator=(const Automaton& initAutom)
{
	if (this == &initAutom)   //defence from self assignment
		return *this;
	delete[]transitions_m;    //if smth was here - need to delete it
	numOfStates_m = initAutom.numOfStates_m;
	numOfActions_m = initAutom.numOfActions_m;
	transitions_m = new Transition[numOfStates_m];
	for (int i = 0; i < numOfStates_m; i++)   //deep copying
	{
		transitions_m[i].fin_m = initAutom.transitions_m[i].fin_m;
		for (int j = 0; j < numOfActions_m; j++)
			transitions_m[i].Actions_m[j] = initAutom.transitions_m[i].Actions_m[j];
	}
	return *this;
}


Automaton Automaton::rightQuotient(const Automaton& secAutomaton) const
{
	Automaton rightQuot = *this;   		  //right quotient of two automata is equal to first atuomaton with new set of final states
	for (int i = 0; i < numOfStates_m; i++)   //make all state non-final
		rightQuot.transitions_m[i].fin_m = 0;
	for (int i = 0; i < numOfStates_m; i++)   //make every state inital and intersec with second automaton
											  //if intersections is not empty - state is final
	{
		rightQuot.transitions_m[i].fin_m = intersecForRightQuot(*this, secAutomaton, i);
	}
	return rightQuot;
}


Automaton Automaton::leftQuotient(const Automaton& secAutomaton) const
{
	Automaton leftQuot = *this;			//left quotient of two automata is equal to first automaton with new set of initial states (can be more than 1)
	Automaton findInitialStates = *this;//Временный полуавтомат для нахождения начальных состояний левого частного
										//Необходим, чтобы не менять исходные полуавтоматы
	for (int i = 0; i < numOfStates_m; i++)				//Делаем все состояния временного полуавтомата финальными
		findInitialStates.transitions_m[i].fin_m = 1;	//
	for (int i = 0; i < numOfStates_m; i++)		//make all states not initial in leftQuotient automaton
		leftQuot.transitions_m[i].init_m = 0;

	int stateOfIntersec = intersecForLeftQuot(*this, secAutomaton);	//При пересечении запоминаем состояние, которое в паре с финальным правого полуавтомата
																	//является финальным (т.е. оба состояния финальные)
	leftQuot.transitions_m[stateOfIntersec].init_m = 1;				//И делаем это состояние начальным в левом полуавтомате
	return leftQuot;
}

//Automaton freindly functions//
bool intersecForRightQuot(const Automaton& leftAutom, const Automaton& rightAutom, int leftInitState)   //1 - there is common behavior (intersection is not empty)
																										//0 - there isn't common behavior (intersection is empty)
{
	struct searchStruct		//additional struct for intersection checking
	{
		int stateFromLeft, stateFromRight;
		bool fin;   			     //1 - if both states are final, else - 0
		bool operator==(const searchStruct& right) const	//for unique checking
		{
			if ((stateFromLeft == right.stateFromLeft) && (stateFromRight == right.stateFromRight))
				return 1;
			else
				return 0;
		}
	};

	searchStruct tmpSearchElement;   	//temporary variable, which we will push into the list
	std::list<searchStruct> searchList;
	tmpSearchElement.stateFromLeft = leftInitState;   //inital state of the left(first automaton)
	tmpSearchElement.stateFromRight = 0;   		  //initial state of the right(second automaton) is always 0
	tmpSearchElement.fin = leftAutom.transitions_m[leftInitState].fin_m && rightAutom.transitions_m[0].fin_m;
	searchList.push_back(tmpSearchElement);   	  //begin of intersection constructing
	for (std::list<searchStruct>::iterator It = searchList.begin(); It != searchList.end(); It++)
	{
		for (int j = 0; j < leftAutom.numOfActions_m; j++)   //while there is no final state
		{
			tmpSearchElement.stateFromLeft = leftAutom.transitions_m[It->stateFromLeft].Actions_m[j];   	//next state in the left automaton by current symbol j
			tmpSearchElement.stateFromRight = rightAutom.transitions_m[It->stateFromRight].Actions_m[j];    //next state in the right automaton by current symbol j
			tmpSearchElement.fin = leftAutom.transitions_m[tmpSearchElement.stateFromLeft].fin_m && rightAutom.transitions_m[tmpSearchElement.stateFromRight].fin_m;
			if (tmpSearchElement.fin)   //if there is final state - intersection is not empty
			{
				return 1;
			}
			if (searchList.end() == std::find(searchList.begin(), searchList.end(), tmpSearchElement))   //otherwise check if it's new pair of states
				searchList.push_back(tmpSearchElement);
		}
	}
	return 0;   //if there are no new uniq pairs of states and no final state - intersection is empty
}

int intersecForLeftQuot(const Automaton& leftAutom, const Automaton& rightAutom, int leftInitState)   //1 - there is common behavior (intersection is not empty)
																									  //0 - there isn't common behavior (intersection is empty)
{
	struct searchStruct		//additional struct for intersection checking
	{
		int stateFromLeft, stateFromRight;
		bool fin;   			     //1 - if both states are final, else - 0
		bool operator==(const searchStruct& right) const	//for unique checking
		{
			if ((stateFromLeft == right.stateFromLeft) && (stateFromRight == right.stateFromRight))
				return 1;
			else
				return 0;
		}
	};

	searchStruct tmpSearchElement;   	//temporary variable, which we will push into the list
	std::list<searchStruct> searchList;
	tmpSearchElement.stateFromLeft = leftInitState;   //inital state of the left(first automaton)
	tmpSearchElement.stateFromRight = 0;   		  //initial state of the right(second automaton) is always 0
	tmpSearchElement.fin = leftAutom.transitions_m[leftInitState].fin_m && rightAutom.transitions_m[0].fin_m;
	searchList.push_back(tmpSearchElement);   	  //begin of intersection constructing
	for (std::list<searchStruct>::iterator It = searchList.begin(); It != searchList.end(); It++)
	{
		for (int j = 0; j < leftAutom.numOfActions_m; j++)   //while there is no final state
		{
			tmpSearchElement.stateFromLeft = leftAutom.transitions_m[It->stateFromLeft].Actions_m[j];   	//next state in the left automaton by current symbol j
			tmpSearchElement.stateFromRight = rightAutom.transitions_m[It->stateFromRight].Actions_m[j];    //next state in the right automaton by current symbol j
			tmpSearchElement.fin = leftAutom.transitions_m[tmpSearchElement.stateFromLeft].fin_m && rightAutom.transitions_m[tmpSearchElement.stateFromRight].fin_m;
			if (tmpSearchElement.fin)   //if there is final state - intersection is not empty
			{
				return tmpSearchElement.stateFromLeft;
			}
			if (searchList.end() == std::find(searchList.begin(), searchList.end(), tmpSearchElement))   //otherwise check if it's new pair of states
				searchList.push_back(tmpSearchElement);
		}
	}
	return 0;   //if there are no new uniq pairs of states and no final state - intersection is empty
}

//bad version
std::ostream& operator<<(std::ostream& os, const Automaton& automaton)
{
	os << "initial(0/1)\t" << "state\t" << "final(0/1)\t" << "actions . . . " << std::endl;
	for (int i = 0; i < automaton.numOfStates_m; i++)
	{
		os << "\t" << automaton.transitions_m[i].init_m << "\t" << i << "\t" << automaton.transitions_m[i].fin_m << "\t\t";
		for (int j = 0; j < automaton.numOfActions_m; j++)
			os << automaton.transitions_m[i].Actions_m[j] << "\t";
		os << std::endl;
	}
	return os;
}

//file output in .fsm format
std::ofstream& operator<<(std::ofstream& ofs, const Automaton& automaton)
{
	ofs << "F 4" << std::endl
		<< "S " << automaton.numOfStates_m << std::endl
		<< "a " << automaton.numOfActions_m << std::endl
		<< "n0 ";
	for (int i = 0; i < automaton.numOfStates_m; i++)	//print initial states
		if (automaton.transitions_m[i].init_m == 1)
			ofs << i << " ";
	ofs << std::endl << "f ";
	for (int i = 0; i < automaton.numOfStates_m; i++)	//print final states
		if (automaton.transitions_m[i].fin_m == 1)
			ofs << i << " ";
	int numOfTransitions = automaton.numOfActions_m*automaton.numOfStates_m;
	ofs << std::endl
		<< "p " << numOfTransitions << std::endl;
	for (int nSts = 0; nSts < automaton.numOfStates_m; nSts++)
	{
		for (int nAct = 0; nAct < automaton.numOfActions_m; nAct++)
			ofs << nSts << " " << nAct << " " << automaton.transitions_m[nSts].Actions_m[nAct] << std::endl;
	}
	return ofs;
}

//file input from .fsm format
std::ifstream& operator >> (std::ifstream& ifs, Automaton& automaton)
{
	if (!ifs)
	{
		std::cerr << "Can't open the file" << std::endl;
		return ifs;
	}
	int state = 0, action = 0, nextState = 0, checkFormat = 0, numOfTransitions = 0;   //temporary variables for data reading
	char currentSymbol;   //current symbol from flow
	ifs >> currentSymbol;   //F skipping
	ifs >> checkFormat;   //4 - automaton
	if (checkFormat != 4)   //error
	{
		std::cerr << "I can work only with automata" << std::endl;
		return ifs;
	}
	ifs >> currentSymbol;   //S skipping
	ifs >> automaton.numOfStates_m;
	ifs >> currentSymbol;   //a skipping
	ifs >> automaton.numOfActions_m;

	delete[] automaton.transitions_m;
	automaton.transitions_m = new Transition[automaton.numOfStates_m];

	for (int i = 0; i < automaton.numOfStates_m; i++)
	{
		automaton.transitions_m[i].Actions_m = new int[automaton.numOfActions_m];
		automaton.transitions_m[i].fin_m = 0;
	}

	for (int j = 0; j < 2; j++)
		ifs >> currentSymbol;   //"n0" skipping

	while (currentSymbol != 'f') 	//we dont't know how much initial states there are
	{
		ifs >> currentSymbol;				//initial states reading
		state = atoi(&currentSymbol);
		automaton.transitions_m[state].init_m = 1;
	}

	while (currentSymbol != 'p')   //we don't know how much final state there are
	{
		state = atoi(&currentSymbol);   		//final states reading
		automaton.transitions_m[state].fin_m = 1;
		ifs >> currentSymbol;
	}
	ifs >> numOfTransitions;

	for (int i = 0; i < numOfTransitions + 1; i++)
	{
		ifs >> state;
		ifs >> action;
		ifs >> nextState;
		automaton.transitions_m[state].Actions_m[action] = nextState;
	}
	return ifs;
}
