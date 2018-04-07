/// @file
/// @brief Description of Automaton and Transition classe.
/// @author Sotnikov A.P.
/// @date 07.04.2018
///
/// <pre>
/// Descriprion of classes Automaton and Transition.
/// Contains left and right quotient.
/// GitHub: https://github.com/SotnikAP/Automata-operations
/// </pre>
#ifndef AUTOMATON_H
#define AUTOMATON_H
#include <iostream>
#include <vector>

namespace automaton
{

/// @brief Structure of transition.
/// 
/// <pre>
/// Transition on it's own is useless, because it doesn't have information about current state.
/// Common view:
/// fin | init | 0 | 1 | ... | N  | --- Transition, where 0, 1, ... N - is indexes of array == actions and elements are states we go from that action.
/// </pre>
struct Transition
{
     bool fin_m;     ///< явл€етс€ ли текущее состо€ние финальным.
     bool init_m;    ///< явл€етс€ ли текущее состо€ние начальным.
     int* Actions_m; ///< ћассива, где индексы == символы действи€, а элементы == состо€ни€, в которые мы перейдем по данному символу.

     Transition();
     ~Transition();
};

/// @brief Describes automaton.
///
/// <pre>
/// Automaton is array of transitions.
/// Array index is current state in transition.
/// Common view of automaton:
/// Cur state | fin | init | 0 | 1 | ... | N
/// </pre>
class Automaton
{

public:
     Automaton();
     Automaton( const Automaton& initAutom );
     ~Automaton();

     /// @brief Makes right quotient. LEFT/RIGHT.
     /// 
     /// @param [in] secAutomaton - right (second) automaton of right quotient. 
     /// @return rightQuotient - automaton which describes right quotient of two initial automata.
     Automaton rightQuotient( const Automaton& secAutomaton ) const;

     /// @brief Makes left quotient. RIGHT/LEFT.
     /// 
     /// @param [in] secAutomaton - right (second) automaton of left quotient. 
     /// @return leftQuotient - automaton which describes left quotient of two initial automata.
     Automaton leftQuotient( const Automaton& secAutomaton ) const;

     Automaton& operator=( const Automaton& v );
     
     /// @brief Writes automaton in the console.
     friend std::ostream& operator<<( std::ostream& os, const Automaton& automaton );

     /// @brief Writes automaton in the file.
     friend std::ofstream& operator<<( std::ofstream& ofs, const Automaton& automaton );

     /// @brief Read automaton from the file.
     friend std::ifstream& operator >> ( std::ifstream& ifs, Automaton& automaton );

private:
     int numOfStates_m;            ///< Number of states in automaton.
     int numOfActions_m;           ///< Number of actions in automaton.
     Transition* transitions_m;    ///< Array of transitions is automaton on itself. Index of array == current state in transition.

private:

     /// @brief Makes intersection, that allows to find final states of right quotient.
     ///
     /// @param [in] rightAutomaton - right (second) automaton of intersection.
     /// @param [in] leftInitState - initial state in left automaton.
     /// @return TRUE if there is common behavior.
     bool intersecForRightQuot( const Automaton& rightAutom, int leftInitState ) const;

     /// @brief Makes intersection, that allows to find initial states of left quotient.
     ///
     /// @param [in] rightAutomaton - right (second) automaton of intersection.
     /// @param [in] leftInitState - initial state in the left automaton (default = 0).
     /// @return initial states vector if left quotient.
     std::vector<int> intersecForLeftQuot( const Automaton& rightAutom, int leftInitState = 0 ) const;
};

}
#endif