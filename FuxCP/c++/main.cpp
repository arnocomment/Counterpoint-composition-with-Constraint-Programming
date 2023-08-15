#include "headers/gecode_problem.hpp"
#include "headers/utilities.hpp"

using namespace Gecode;
using namespace std;

int main(int argc, char* argv[]) {
    std::cout << "starting main" << std::endl;
    // 4 lists, one per beat and each element is the measure
    const int nRows = 4;      // TODO next : take these values from argv
    const int nCols = 11;     // number of measures
    vector<vector<int>> cantus_firmus = {
        {62,65,64,62,67,65,69,67,65,64,62}, // no elements in other beats since this is first species
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    // create a new problem
    
    Problem* p = new Problem(cantus_firmus, nCols);

    // create a new search engine
    Search::Base<Problem>* e = make_solver(p, DFS_SOLVER);
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

    return 0;
    

}