/// @file
/// @brief Realization of Automaton and Transition classes.
/// @author Sotnikov A.P.
/// @date 07.04.2018
///
/// <pre>
/// Realization of classes Automaton and Transition.
/// GitHub: https://github.com/SotnikAP/Automata-operations
/// </pre>
#include <list>
#include <algorithm>
#include <fstream>
#include <cstdlib>   //for atoi
#include <vector>
#include "Automaton.h"

typedef std::vector<int>::const_iterator vecConstIt;

using namespace automaton;

Transition::Transition()
{
     fin_m = 0;
     init_m = 0;
     Actions_m = nullptr;
}

Transition::~Transition()
{
     delete[] Actions_m;
}


Automaton::Automaton()
{
     numOfStates_m = 0;
     numOfActions_m = 0;
     transitions_m = nullptr;
}

Automaton::Automaton( const Automaton & initAutom )
{
     numOfActions_m = initAutom.numOfActions_m;
     numOfStates_m = initAutom.numOfStates_m;
     transitions_m = new Transition[numOfStates_m];   //memory allocation for array of transitions
     for ( int i = 0; i < numOfStates_m; i++ )   	    //deep copying
     {
          transitions_m[i].fin_m = initAutom.transitions_m[i].fin_m;
          transitions_m[i].init_m = initAutom.transitions_m[i].init_m;
          transitions_m[i].Actions_m = new int[numOfActions_m];
          for ( int j = 0; j < numOfActions_m; j++ )
          {
               transitions_m[i].Actions_m[j] = initAutom.transitions_m[i].Actions_m[j];
          }
     }
}

Automaton::~Automaton()
{
     delete[]transitions_m;
}

Automaton& Automaton::operator=( const Automaton& initAutom )
{
     if ( this == &initAutom )
     {
          return *this;
     }
     delete[]transitions_m;
     numOfStates_m = initAutom.numOfStates_m;
     numOfActions_m = initAutom.numOfActions_m;
     transitions_m = new Transition[numOfStates_m];
     for ( int i = 0; i < numOfStates_m; i++ )
     {
          transitions_m[i].fin_m = initAutom.transitions_m[i].fin_m;
          for ( int j = 0; j < numOfActions_m; j++ )
               transitions_m[i].Actions_m[j] = initAutom.transitions_m[i].Actions_m[j];
     }
     return *this;
}

Automaton Automaton::rightQuotient( const Automaton& secAutomaton ) const
{
     Automaton rightQuot = *this; //< Equal to left automaton (this) with new set of final states.
     for ( int i = 0; i < numOfStates_m; i++ )
          // Makes all states not final.
          // We will find new set of final states.
     {
          rightQuot.transitions_m[i].fin_m = 0;
     }

     for ( int i = 0; i < numOfStates_m; i++ )
          // Make every state inital and intersect with second automaton.
          // If intersection is not empty - this initial state is final in right quotient.
     {
          rightQuot.transitions_m[i].fin_m = intersecForRightQuot( secAutomaton, i );
     }

     return rightQuot;
}

Automaton Automaton::leftQuotient( const Automaton& secAutomaton ) const
{
     Automaton leftQuot = *this;          //< Equal to left automaton (this) with new set if initial states.		
     Automaton findInitialStates = *this; //< Temporary automaton, so we do not update initial automaton.
                                                  
     for ( int i = 0; i < numOfStates_m; i++ )
          // Makes all states final and not-inital, so we can find true initial states in left quotient.
     {
          findInitialStates.transitions_m[i].fin_m = 1;
          leftQuot.transitions_m[i].init_m = 0;
     }

     std::vector<int> initStates = findInitialStates.intersecForLeftQuot( secAutomaton ); //< Initial states of left quotient.

     for ( vecConstIt cIt = initStates.begin(); cIt != initStates.end(); ++cIt )
          // Makes states initial.
     {
          leftQuot.transitions_m[*cIt].init_m = 1;
     }

     return leftQuot;
}

/// @brief Sructure for finding common behavior of two automata.
///
/// Is used for intersections for left and right quotients.
/// New elements comes from previos.
struct searchStruct
{
     int stateFromLeft, stateFromRight;
     bool fin;                          ///< True if both states are final.
     bool operator==( const searchStruct& right ) const
          // To find uniq pairs of states.
     {
          return ( stateFromLeft == right.stateFromLeft ) && ( stateFromRight == right.stateFromRight ) ? 1 : 0;
     }
};

bool Automaton::intersecForRightQuot( const Automaton& rightAutom, int leftInitState ) const                                                                                                         
{
     searchStruct tmpSearchElement;   	               // Temporary variable, which we will push into the list.
     std::list<searchStruct> searchList;
     tmpSearchElement.stateFromLeft = leftInitState;   // Initial state of the left(first automaton)
     tmpSearchElement.stateFromRight = 0;   		     // Initial state of the right(second automaton) is always 0
     tmpSearchElement.fin = transitions_m[leftInitState].fin_m && rightAutom.transitions_m[0].fin_m;
     searchList.push_back( tmpSearchElement );   	     // Begin of intersection constructing.
     for ( std::list<searchStruct>::iterator It = searchList.begin(); It != searchList.end(); ++It )
     {
          for ( int j = 0; j < numOfActions_m; j++ )   // While there is no final state.
          {
               tmpSearchElement.stateFromLeft = transitions_m[It->stateFromLeft].Actions_m[j];   	          // Next state in the left automaton by current symbol j.
               tmpSearchElement.stateFromRight = rightAutom.transitions_m[It->stateFromRight].Actions_m[j];   // Next state in the right automaton by current symbol j.
               tmpSearchElement.fin = transitions_m[tmpSearchElement.stateFromLeft].fin_m && rightAutom.transitions_m[tmpSearchElement.stateFromRight].fin_m;
               if ( tmpSearchElement.fin )
                    // If there is final state - intersection is not empty.
               {
                    return 1;
               }
               if ( searchList.end() == std::find( searchList.begin(), searchList.end(), tmpSearchElement ) )
                    // Otherwise check if it's new pair of states.
                    // std::find returns last element if there is no such elemnt.
               {
                    searchList.push_back( tmpSearchElement );
               }
          }
     }
     return 0;   // If there are no new unique pairs of states and no common behavior.
}

std::vector<int> Automaton::intersecForLeftQuot( const Automaton& rightAutom, int leftInitState ) const   
{
     searchStruct tmpSearchElement;   	               // Temporary variable, which we will push into the list.
     std::list<searchStruct> searchList;
     tmpSearchElement.stateFromLeft = leftInitState;   // Inital state of the left(first automaton).
     tmpSearchElement.stateFromRight = 0;   		     // Initial state of the right(second automaton) is always 0.
     tmpSearchElement.fin = transitions_m[leftInitState].fin_m && rightAutom.transitions_m[0].fin_m;
     searchList.push_back( tmpSearchElement );   	     // Begin of intersection constructing.
     std::vector<int> initStates (0);
     for ( std::list<searchStruct>::iterator It = searchList.begin(); It != searchList.end(); ++It )
     {
          for ( int j = 0; j < numOfActions_m; j++ )   // While there is no final state.
          {
               tmpSearchElement.stateFromLeft = transitions_m[It->stateFromLeft].Actions_m[j];   	           // Next state in the left automaton by current symbol j.
               tmpSearchElement.stateFromRight = rightAutom.transitions_m[It->stateFromRight].Actions_m[j];    // Next state in the right automaton by current symbol j.
               tmpSearchElement.fin = transitions_m[tmpSearchElement.stateFromLeft].fin_m && rightAutom.transitions_m[tmpSearchElement.stateFromRight].fin_m;
               if ( tmpSearchElement.fin )
                    // If there is final state - this state is initial.
               {
                    initStates.push_back( tmpSearchElement.stateFromLeft );
                    break;
               }
               if ( searchList.end() == std::find( searchList.begin(), searchList.end(), tmpSearchElement ) )   //otherwise check if it's new pair of states
               {
                    searchList.push_back( tmpSearchElement );
               }
          }
     }
     return initStates;   // If there are no new unique pairs of states and no final state - intersection is empty.
}

/// @TODO: Add formating.
std::ostream& automaton::operator<<( std::ostream& os, const Automaton& automaton )
{
     os << "initial(0/1)\t" << "state\t" << "final(0/1)\t" << "actions . . . " << std::endl;
     for ( int i = 0; i < automaton.numOfStates_m; i++ )
     {
          os << "\t" << automaton.transitions_m[i].init_m << "\t" << i << "\t" << automaton.transitions_m[i].fin_m << "\t\t";
          for ( int j = 0; j < automaton.numOfActions_m; j++ )
               os << automaton.transitions_m[i].Actions_m[j] << "\t";
          os << std::endl;
     }
     return os;
}

std::ofstream& automaton::operator<<( std::ofstream& ofs, const Automaton& automaton )
{
     ofs << "F 4" << std::endl
          << "S " << automaton.numOfStates_m << std::endl
          << "a " << automaton.numOfActions_m << std::endl
          << "n0 ";
     for ( int i = 0; i < automaton.numOfStates_m; i++ )
          // Print initial states.
     {
          if ( automaton.transitions_m[i].init_m == 1 )
          {
               ofs << i << " ";
          }
     }
     ofs << std::endl << "f ";
     for ( int i = 0; i < automaton.numOfStates_m; i++ )
          // Print final states.
     {
          if ( automaton.transitions_m[i].fin_m == 1 )
          {
               ofs << i << " ";
          }
     }
     int numOfTransitions = automaton.numOfActions_m*automaton.numOfStates_m;
     ofs << std::endl
          << "p " << numOfTransitions << std::endl;
     for ( int nSts = 0; nSts < automaton.numOfStates_m; nSts++ )
     {
          for ( int nAct = 0; nAct < automaton.numOfActions_m; nAct++ )
          {
               ofs << nSts << " " << nAct << " " << automaton.transitions_m[nSts].Actions_m[nAct] << std::endl;
          }
     }
     return ofs;
}

std::ifstream& automaton::operator>>( std::ifstream& ifs, Automaton& automaton )
{
     if ( !ifs )
     {
          std::cerr << "Can't open the file" << std::endl;
          return ifs;
     }
     int state = 0, action = 0, nextState = 0, checkFormat = 0, numOfTransitions = 0;   //temporary variables for data reading
     char currentSymbol;           //< Current symbol from flow.
     ifs >> currentSymbol;         // skips 'F'.
     ifs >> checkFormat;           
     if ( checkFormat != 4 )
          // 4 == automaton
     {
          std::cerr << "I can work only with automata" << std::endl;
          return ifs;
     }
     ifs >> currentSymbol;         // Skips 'S'.
     ifs >> automaton.numOfStates_m;
     ifs >> currentSymbol;         // Skips 'a'.
     ifs >> automaton.numOfActions_m;

     delete[] automaton.transitions_m;
     automaton.transitions_m = new Transition[automaton.numOfStates_m];

     for ( int i = 0; i < automaton.numOfStates_m; i++ )
     {
          automaton.transitions_m[i].Actions_m = new int[automaton.numOfActions_m];
          automaton.transitions_m[i].fin_m = 0;
     }

     for ( int j = 0; j < 2; j++ )
          ifs >> currentSymbol;   // Skips 'n0'.

     while ( currentSymbol != 'f' )
          // Reads all initial states.
     {
          ifs >> state;
          if ( !ifs )
          {
               ifs.clear();
               ifs >> currentSymbol;
          }
          automaton.transitions_m[state].init_m = 1;
     }

     while ( currentSymbol != 'p' )
          // Reads all final states.
     {
          ifs >> state;
          if ( !ifs )
          {
               ifs.clear();
               ifs >> currentSymbol;
          }
          automaton.transitions_m[state].fin_m = 1;
     }
     ifs >> numOfTransitions;

     for ( int i = 0; i < numOfTransitions + 1; i++ )
     {
          ifs >> state;
          ifs >> action;
          ifs >> nextState;
          automaton.transitions_m[state].Actions_m[action] = nextState;
     }
     return ifs;
}