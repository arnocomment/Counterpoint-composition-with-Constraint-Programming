#include "headers/gecode_problem.hpp"
#include "headers/utilities.hpp"

/***********************************************************************************************************************
 *                                          Problem class methods                                                      *
 ***********************************************************************************************************************/

/**
 * Constructor
 * @param cantus_firmus Contains the MIDI values of the cantus firmus
 */
Problem::Problem (vector<vector<int>> cantus_firmus, int nMeasures) {
    string message = "WSpace object created. ";

    /*************************************************************************************************************
    *                                          INIT FOR ALL SPECIES                                             *
    **************************************************************************************************************/
    size = nMeasures*4;                                     // Only 4/4 is supported
    MAX_NOTES_CP = 3*(nMeasures-1) + nMeasures;             // from p.12 //TODO useful? 

    // BUILD SET OF VALUES FOR THE NOTES IN CP (see N, p.16)
    vector<int> basic_scale = scales[IONIAN];                               // get intervals to build scale
    vector<int> borrowed_scale = scales[BORROWED];                          // intervals for additional values depending on tonic (first node of cantus firmus)
    vector<int> scale = get_all_notes_from_scale(C, basic_scale);           // all notes from C major (tmp)
    vector<int> borrowed_notes = get_all_notes_from_scale(cantus_firmus[0][0] % 12, borrowed_scale);    // get all additional borrowed notes
    

    // TODO : filter vector in given range -> best way is to give this range as arguments of get_all_notes_from_scale
    set<int> merge_scales(scale.begin(), scale.end());                      // merge all notes together 
    merge_scales.insert(borrowed_notes.begin(), borrowed_notes.end());      // and delete duplicates
    vector<int> domain_vector(merge_scales.begin(), merge_scales.end());    // cast back to vector
    
    domain_vector.erase(remove_if(domain_vector.begin(), domain_vector.end(), [](int value) {
        return value < 21;                                                  // remove all values below A0 (21)
    }), domain_vector.end());
    domain_vector.push_back(0);                                             // 0 = no note on this beat

    lower_bound_domain = 0;                                                 // lowest possible note  = no note
    upper_bound_domain = domain_vector.back();                              // highest possible note for cp

    
    /*************************************************************************************************************
    *                                          SPECIFIC TO SPECIES 1                                             *
    **************************************************************************************************************/
    // INITIALIZATION SPECIFIC TO SPECIES 1
    int b = 1;                                              // from p.13, number of beats in a measure used by the solver
    int d = 4/b;                                            // duration of a note, depending on chosen species


    /*************************************************************************************************************
    *                                          INIT VARIABLES (CP)                                               *
    **************************************************************************************************************/
    IntSet domain(domain_vector.data(), domain_vector.size());         // convert vector to IntSet
    vars = IntVarArray(*this, size, domain);                           // CP (notes of the counterpoint)
    Matrix<IntVarArray> cp(vars, nMeasures, 4);                        // Matrix view of CP (wrapper)


    // H intervals            
    h_intervals_abs_vars = IntVarArray(*this, size, 0, 18);                         // init h_abs (range is tmp)
    Matrix<IntVarArray> h_intervals_abs(h_intervals_abs_vars, nMeasures, 4);        // Matrix view of CP (wrapper)
    for (int i = 0; i < 4; ++i) {                                                   // rows
        for (int j = 0; j < nMeasures; ++j) {                                       // columns
            if (cantus_firmus[i][j] == 0) {
                rel(*this, cp(j, i), IRT_EQ, cantus_firmus[i][j]);
            }
            else {
                rel(*this, vars[i * nMeasures + j], IRT_NQ, 0);
                rel(*this, h_intervals_abs(j, i) == abs(cp(j, i) - cantus_firmus[i][j]));    // in gecode m(j, i) is for column j and row i
            }
        }
    }
    h_intervals_vars = IntVarArray(*this, size, 0, 11);
    for (int i = 0; i < size; ++i) {
        rel(*this, h_intervals_vars[i] == h_intervals_abs_vars[i] % 12);
    }


    // M intervals
    m_intervals = IntVarArray(*this, nMeasures - 1, -12, 12);           // tmp size for species 1
    m_intervals_abs = IntVarArray(*this, nMeasures - 1, 0, 12);
    for (int j = 0; j < nMeasures - 1; ++j) {
        rel(*this, m_intervals[j] == cp(j+1, 0) - cp(j, 0));
        rel(*this, m_intervals_abs[j] == abs(m_intervals[j]));
    }

    // P motions
    p_motions = IntVarArray(*this, nMeasures - 1, 0, 2);
    for (int i = 0; i < m_intervals.size(); i++) {
        bool_motions_cp_up << BoolVar(*this, 0, 1);                     // add boolean variable (0 false, 1 true)
        bool_motions_cp_down << BoolVar(*this, 0, 1);
        bool_motions_cp_static << BoolVar(*this, 0, 1);
        rel(*this, m_intervals[i] > 0 == bool_motions_cp_up[i]);        // 1 if M_intervals > 0
        rel(*this, m_intervals[i] < 0 == bool_motions_cp_down[i]);      // 1 of M_intervals < 0
        rel(*this, (m_intervals[i] == 0) == bool_motions_cp_static[i]);
        
    }
    vector<int> m_cf(nMeasures - 1);
    for (int j = 0; j < nMeasures - 1; ++j) {
        m_cf[j] = cantus_firmus[0][j+1] - cantus_firmus[0][j];
        rel(*this, p_motions[j], IRT_EQ, OBLIQUE, bool_motions_cp_static[j]);
        Reify r_up(bool_motions_cp_up[j], RM_IMP);
        if (m_cf[j] < 0) {
            rel(*this, p_motions[j], IRT_EQ, CONTRARY, r_up);
            rel(*this, p_motions[j], IRT_EQ, DIRECT, bool_motions_cp_down[j]);
        }
        else if (m_cf[j] > 0) {
            rel(*this, p_motions[j], IRT_EQ, DIRECT, bool_motions_cp_up[j]);
            rel(*this, p_motions[j], IRT_EQ, CONTRARY, bool_motions_cp_down[j]);
        }
        else {
            rel(*this, p_motions[j], IRT_EQ, OBLIQUE);
        }
    }
    

    /*************************************************************************************************************
    *                                               CONSTRAINTS                                                  *
    **************************************************************************************************************/
    

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
    h_intervals_abs_vars.update(*this, s.h_intervals_abs_vars);
    h_intervals_vars.update(*this, s.h_intervals_vars);
    m_intervals.update(*this, s.m_intervals);
    m_intervals_abs.update(*this, s.m_intervals_abs);
    p_motions.update(*this, p_motions);
    bool_motions_cp_up = s.bool_motions_cp_up;
    bool_motions_cp_down = s.bool_motions_cp_down;
    bool_motions_cp_static = s.bool_motions_cp_static;

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
    //cout << h_intervals_vars << endl;
    //cout << h_intervals_abs_vars << endl;
    cout << m_intervals << endl;
    //cout << m_intervals_abs << endl;
    cout << p_motions << endl;
    cout << bool_motions_cp_up << endl;
    cout << bool_motions_cp_down << endl;
    cout << bool_motions_cp_static << endl;
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