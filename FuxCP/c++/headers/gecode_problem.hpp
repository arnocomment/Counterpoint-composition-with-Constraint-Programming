#ifndef space_wrapper_hpp
#define space_wrapper_hpp

#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <ctime>
#include <exception>
#include <cmath>
#include <cctype>

#include "gecode/driver.hh"
#include "gecode/search.hh"
#include "gecode/minimodel.hh"
#include "gecode/set.hh"
#include "gecode/int.hh"
#include "gecode/kernel.hh"


#include "utilities.hpp"

using namespace Gecode;
using namespace Gecode::Search;
using namespace std;


/*****************
 * Problem class *
 *****************/
 // This class represents a constraint problem to be solved
class Problem: public Space {
protected:
    // solution related attributes
    IntVarArray vars;                       // CP     (notes of the counterpoint)
    IntVarArray h_intervals_vars;           // H      (harmonic intervals between H and CP, modulo 12)
    IntVarArray h_intervals_abs_vars;       // H_abs  (harmonic intervals between H and CP)
    IntVarArray m_intervals;                // M_brut (melodic intervals in CP)           
    IntVarArray m_intervals_abs;            // M_abs  (absolute value of melodic intervals in CP)
    IntVarArray p_motions;                  // P      (motions)
    BoolVarArgs bool_motions_cp_up;
    BoolVarArgs bool_motions_cp_down;
    BoolVarArgs bool_motions_cp_static;
    int size; // The size of the variable array of interest
    int lower_bound_domain;
    int upper_bound_domain;
    int MAX_NOTES_CP;           // maximum amount of notes in counterpoint //TODO useful?

    /*************************************************************************************
     *
     *                                  GLOBAL CONTSTANTS
     *
    **************************************************************************************/

    

public:
    /**
     * Constructor
     * @param cantus_firmus Contains the MIDI values of the cantus firmus
     */
    Problem(vector<vector<int>> cantus_firmus, int nMeasures);

    /**
     * Copy constructor
     * @param s an instance of the Problem class
     * @todo modify this copy constructor to also copy any additional attributes you add to the class
     */
    Problem(Problem &s);

    /**
     * Returns the size of the problem
     * @return an integer representing the size of the vars array
     */
    int getSize();

    /**
     * Returns the values taken by the variables vars in a solution
     * @todo Modify this to return the solution for your problem. This function uses @param size to generate an array of integers
     * @return an array of integers representing the values of the variables in a solution
     */
    int* return_solution();

    /**
     * Copy method
     * @return a copy of the current instance of the Problem class. Calls the copy constructor
     */
    virtual Space *copy(void);

    /**
     * Constrain method for bab search
     * @todo modify this function if you want to use branch and bound
     * @param _b a space to constrain the current instance of the Problem class with upon finding a solution
     */
    virtual void constrain(const Space& _b);

    /**
     * Prints the solution in the console
     */
    void print_solution();

    /**
     * toString method
     * @return a string representation of the current instance of the Problem class.
     * Right now, it returns a string "Problem object. size = <size>"
     * If a variable is not assigned when this function is called, it writes <not assigned> instead of the value
     * @todo modify this method to also print any additional attributes you add to the class
     */
    string toString();
};


/*************************
 * Search engine methods *
 *************************/

/**
 * Creates a search engine for the given problem
 * @todo Modify this function to add search options etc
 * @param pb an instance of the Problem class representing a given problem
 * @param type the type of search engine to create (see enumeration in headers/gecode_problem.hpp)
 * @return a search engine for the given problem
 */
Search::Base<Problem>* make_solver(Problem* pb, int type);

/**
 * Returns the next solution space for the problem
 * @param solver a solver for the problem
 * @return an instance of the Problem class representing the next solution to the problem
 */
Problem* get_next_solution_space(Search::Base<Problem>* solver);


/***********************
 * Auxiliary functions *
 ***********************/

/**
 * Write a text into a log file
 * @param message the text to write
 */
void writeToLogFile(const char* message);

#endif