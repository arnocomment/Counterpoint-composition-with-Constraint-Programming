#include "headers/gecode_problem.hpp"
#include "headers/utilities.hpp"

using namespace Gecode;
using namespace std;

int main(int argc, char* argv[]) {
    std::cout << "starting main" << std::endl;
    // 4 lists, one per beat and each element is the measure
    const int nRows = 4;      // TODO next : take these values from argv
    const int nCols = 11;     // number of measures
    int cantus_firmus_values[nRows][nCols] = {
        {62,65,64,62,67,65,69,67,65,64,62}, // no elements in other beats since this is first species
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    // allocate memory for 2D array (array of pointers to int arrays)
    int** cantus_firmus = new int*[nRows];
    for (int i = 0; i < nRows; ++i) {
        cantus_firmus[i] = new int[nCols];
    }

    // Initialize the array 
    for (int i = 0; i < nRows; ++i) {
        for (int j = 0; j < nCols; ++j) {
            cantus_firmus[i][j] = cantus_firmus_values[i][j];
        }
    }

    
    // using 1D array for better cache performance? -> access elements with array[x*cols + y] ?
    // create a new problem
    
    Problem* p = new Problem(cantus_firmus, nCols);

    // create a new search engine
    Search::Base<Problem>* e = make_solver(p, BAB_SOLVER);
    delete p;

    int nb_sol = 0;
    int i=0;
    while(Problem * pb = get_next_solution_space(e)){   // give 20 solutions
        nb_sol++;
        cout << "Solution " << nb_sol << ": " << endl;
        pb->print_solution();
        delete pb;
        ++i;
        if (i == 20) {
            break;
        }
    }
    cout << "No (more) solutions." << endl;

    // Clean up memory
    for (int i = 0; i < nRows; ++i) {
        delete[] cantus_firmus[i];
    }
    delete[] cantus_firmus;

    return 0;
    

}