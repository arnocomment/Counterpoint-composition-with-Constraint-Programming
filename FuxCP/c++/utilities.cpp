#include "headers/utilities.hpp"


map<int, const vector<int>> scales = {
    {IONIAN, {MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND}},         // DO  - C
    {DORIAN, {MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND}},         // RE  - D
    {PHRYGIAN, {MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND}},       // MI  - E
    {LYDIAN, {MAJOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND}},         // FA  - F
    {MIXOLYDIAN, {MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND}},     // SOL - G
    {AEOLIAN, {MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND}},        // LA  - A
    {LOCRIAN, {MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MAJOR_SECOND}},        // SI  - B
    {HARMONIC_MINOR, {MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND, MINOR_THIRD, MINOR_SECOND}},
    {PENTATONIC, {MAJOR_SECOND, MAJOR_SECOND, MINOR_THIRD, MAJOR_SECOND, MINOR_THIRD}},
    {CHROMATIC, {MINOR_SECOND, MINOR_SECOND, MINOR_SECOND, MINOR_SECOND, MINOR_SECOND, MINOR_SECOND, MINOR_SECOND, MINOR_SECOND, MINOR_SECOND, MINOR_SECOND, MINOR_SECOND, MINOR_SECOND}},
    {BORROWED, {PERFECT_FOURTH, MAJOR_THIRD, MAJOR_SECOND, MINOR_SECOND}}
};

/**
 * For a given set of intervals between notes that loops and a starting note, returns all the possible notes
 * @param note the starting note
 * @param intervals the set of intervals between notes. It must make a loop. For example, to get all notes from a major
 * scale from note, use {2, 2, 1, 2, 2, 2, 1}. To get all notes from a minor chord, use {3, 4, 5}.
 * @return vector<int> all the notes
 */
vector<int> get_all_notes_from_interval_loop(int n, vector<int> intervals)
{
    int note = n % PERFECT_OCTAVE; // bring the root back to [12,23] in case the argument is wrong
    vector<int> notes;

    int i = 0;
    while (note <= 127)
    {
        notes.push_back(note);
        note += intervals[i % intervals.size()];
        ++i;
    }
    return notes;
}

/**
 * For a given tonality (root + mode), returns all the possible notes
 * @param root the root of the tonality (in [0,11])
 * @param scale the set of tones and semitones that define the scale
 * @return vector<int> all the possible notes from that tonality
 */
vector<int> get_all_notes_from_scale(int root, vector<int> scale)
{
    return get_all_notes_from_interval_loop(root, scale);
}