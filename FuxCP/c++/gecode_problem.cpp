#include "headers/gecode_problem.hpp"
#include "headers/utilities.hpp"

/***********************************************************************************************************************
 *                                          Problem class methods                                                      *
 ***********************************************************************************************************************/

/**
 * Constructor
 * @param cantus_firmus Contains the MIDI values of the cantus firmus
 */
Problem::Problem (int** cantus_firmus, int nMeasures) {
    string message = "WSpace object created. ";

    /*************************************************************************************************************
    *                                          INIT FOR ALL SPECIES                                             *
    **************************************************************************************************************/
    size = nMeasures*4;                                     // Only 4/4 is supported
    MAX_NOTES_CP = 3*(nMeasures-1) + nMeasures;             // from p.12 //TODO useful? 

    // BUILD SET OF VALUES FOR THE NOTES IN CP (see N, p.16)
    vector<int> scale = scales[IONIAN];                             
    vector<int> domain_vector = get_all_notes_from_scale(C, scale);        // for standard example in C major
    // TODO : filter vector in given range -> best way is to give this range as arguments of get_all_notes_from_scale
    lower_bound_domain = domain_vector.front();                            // minimum MIDI value for counterpoint
    upper_bound_domain = domain_vector.back();                             // maximum MIDI value for counterpoint

    
    /*************************************************************************************************************
    *                                          SPECIFIC TO SPECIES 1                                             *
    **************************************************************************************************************/
    // INITIALIZATION SPECIFIC TO SPECIES 1
    int b = 1;                                              // from p.13, number of beats in a measure used by the solver
    int d = 4/b;                                            // duration of a note, depending on chosen species


    /*************************************************************************************************************
    *                                          INIT VARIABLES (CP)                                               *
    **************************************************************************************************************/
    IntSet domain(domain_vector.data(), domain_vector.size());                  // convert vector to IntSet
    vars = IntVarArray(*this, size, domain);   
    Matrix<IntVarArray> cp(vars, 11, 4);                                              

    /*************************************************************************************************************
    *                                               CONSTRAINTS                                                  *
    **************************************************************************************************************/
    //G4 : "The counterpoint must be in the same key as the cantus firmus"
    distinct(*this, vars); // tmp
    

    //branching
    branch(*this, vars, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    writeToLogFile(message.c_str());
}


/**
 * Copy constructor
 * @param s an instance of the Problem class
 * @todo modify this copy constructor to also copy any additional attributes you add to the class
 */
Problem::Problem(Problem& s): Space(s){
    //IntVars update
    size = s.size;
    lower_bound_domain = s.lower_bound_domain;
    upper_bound_domain = s.upper_bound_domain;
    MAX_NOTES_CP = s.MAX_NOTES_CP;
    vars.update(*this, s.vars);
    // ERREUR BIZARRE CHECK ICI
}

/**
 * Returns the size of the problem
 * @return an integer representing the size of the vars array
 */
int Problem::getSize(){
    string message = "getSize function called. size = " + to_string(size) + "\n";
    writeToLogFile(message.c_str());
    return size;
}

/**
 * Returns the values taken by the variables vars in a solution
 * @todo Modify this to return the solution for your problem. This function uses @param size to generate an array of integers
 * @return an array of integers representing the values of the variables in a solution
 */
int* Problem::return_solution(){
    string message = "return_solution method. Solution : [";
    int* solution = new int[size];
    for(int i = 0; i < size; i++){
        solution[i] = vars[i].val();
        message += to_string(solution[i]) + " ";
    }
    message += "]\n";
    writeToLogFile(message.c_str());
    return solution;
}

/**
 * Copy method
 * @return a copy of the current instance of the Problem class. Calls the copy constructor
 */
Space* Problem::copy(void) {
    return new Problem(*this);
}

/**
 * Constrain method for bab search
 * @todo modify this function if you want to use branch and bound
 * @param _b a solution to the problem from which we wish to add a constraint for the next solutions
 */
void Problem::constrain(const Space& _b) {
    const Problem &b = static_cast<const Problem &>(_b);
    rel(*this, vars, IRT_GQ, 2);
}

/**
 * Prints the solution in the console
 */
void Problem::print_solution(){
    for(int i = 0; i < size; i++){
        cout << vars[i].val() << " ";
    }
    cout << endl;
}

/**
 * toString method
 * @return a string representation of the current instance of the Problem class.
 * Right now, it returns a string "Problem object. size = <size>"
 * If a variable is not assigned when this function is called, it writes <not assigned> instead of the value
 * @todo modify this method to also print any additional attributes you add to the class
 */
string Problem::toString(){
    string message = "Problem object. \n";
    message += "size = " + to_string(size) + "\n" + "lower bound for the domain : " +
            to_string(lower_bound_domain) + "\n" + "upper bound for the domain : " + to_string(upper_bound_domain)
             + "\n" + "current values for vars: [";
    for(int i = 0; i < size; i++){
        if (vars[i].assigned())
            message += to_string(vars[i].val()) + " ";
        else
            message += "<not assigned> ";
    }
    message += "]\n\n";
    writeToLogFile(message.c_str());
    return message;
}

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
Search::Base<Problem>* make_solver(Problem* pb, int type){
    string message = "make_solver function called. type of solver :\n" + to_string(type) + "\n";
    writeToLogFile(message.c_str());

    Gecode::Search::Options opts;
    /**@todo add here any options you want*/

    if (type == BAB_SOLVER)
        return new BAB<Problem>(pb, opts);
    else // default case
        return new DFS<Problem>(pb, opts);
}

/**
 * Returns the next solution space for the problem
 * @param solver a solver for the problem
 * @return an instance of the Problem class representing the next solution to the problem
 */
Problem* get_next_solution_space(Search::Base<Problem>* solver){
    string message = "get_next_solution_space function called.\n";
    Problem* sol_space = solver->next();
    if (sol_space == nullptr)
        return NULL;
    message += sol_space->toString();
    writeToLogFile(message.c_str());
    return sol_space;
}

/***********************
 * Auxiliary functions *
 ***********************/

/**
 * Write a text into a log file
 * @param message the text to write
 */
void writeToLogFile(const char* message){
    std::time_t currentTime = std::time(nullptr); // Get the current time
    std::string timeString = std::asctime(std::localtime(&currentTime)); // Convert to string

    const char* homeDir = std::getenv("HOME"); // Get the user's home directory
    if (homeDir) {
        std::string filePath(homeDir);
        filePath += "/log.txt"; // Specify the desired file path, such as $HOME/log.txt

        std::ofstream myfile(filePath, std::ios::app); // append mode
        if (myfile.is_open()) {
            myfile <<timeString<< endl << message << endl;
            myfile.close();
        }
    }
}