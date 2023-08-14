#ifndef UTILITIES
#define UTILITIES

#include <gecode/int.hh>
#include <gecode/minimodel.hh>

#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;
using namespace Gecode;

/***********************************************************************************************************************
 *                                                  Useful constants                                                   *
 ***********************************************************************************************************************/

/** Types of search engines */
enum solver_types{
    DFS_SOLVER, //0
    BAB_SOLVER, //1
};

const vector<std::string> noteNames = {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"}; // @todo turn into a bi-directional map

/** Notes */
const int B_SHARP = 0;
const int C = 0;
const int C_SHARP = 1;
const int D_FLAT = 1;
const int D = 2;
const int D_SHARP = 3;
const int E_FLAT = 3;
const int E = 4;
const int F_FLAT = 4;
const int E_SHARP = 5;
const int F = 5;
const int F_SHARP = 6;
const int G_FLAT = 6;
const int G = 7;
const int G_SHARP = 8;
const int A_FLAT = 8;
const int A = 9;
const int A_SHARP = 10;
const int B_FLAT = 10;
const int B = 11;
const int C_FLAT = 11;


enum Mode {
    IONIAN,     //0 , major mode
    DORIAN,     //1
    PHRYGIAN,   //2
    LYDIAN,     //3
    MIXOLYDIAN, //4
    AEOLIAN,    //5 , natural minor mode
    LOCRIAN,    //6
    HARMONIC_MINOR, //7
    PENTATONIC, //8
    CHROMATIC,  //9
    BORROWED    //10
};

enum Motions {
    CONTRARY,
    DIRECT,
    OBLIQUE
};


enum intervals{
    UNISSON,            //0
    MINOR_SECOND,       //1
    MAJOR_SECOND,       //2
    MINOR_THIRD,        //3
    MAJOR_THIRD,        //4
    PERFECT_FOURTH,     //5
    TRITONE,            //6
    PERFECT_FIFTH,      //7
    MINOR_SIXTH,        //8
    MAJOR_SIXTH,        //9
    MINOR_SEVENTH,      //10
    MAJOR_SEVENTH,      //11
    PERFECT_OCTAVE      //12
};

extern map<int, const vector<int>> scales; 


/**
 * For a given set of intervals between notes that loops and a starting note, returns all the possible notes
 * @param note the starting note
 * @param intervals the set of intervals between notes. It must make a loop. For example, to get all notes from a major
 * scale from note, use {2, 2, 1, 2, 2, 2, 1}. To get all notes from a minor chord, use {3, 4, 5}.
 * @return vector<int> all the notes
 */
vector<int> get_all_notes_from_interval_loop(int n, vector<int> intervals);


/**
 * For a given tonality (root + mode), returns all the possible notes
 * @param root the root of the tonality (in [0,11])
 * @param scale the set of tones and semitones that define the scale
 * @return vector<int> all the possible notes from that tonality
 */
vector<int> get_all_notes_from_scale(int root, vector<int> scale);



#endif