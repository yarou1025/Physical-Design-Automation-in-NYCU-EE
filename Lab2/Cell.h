#ifndef CELL_H
#define CELL_H

#include <vector>
using namespace std;
typedef struct Cell
{
    int from;
    int to;
    int gain;
    int is_locked;
    int in_part;
    vector<int> in_net;
} Cell;

#define From(c) ((c).from)
#define To(c) ((c).to)
#define Locked(c) ((c).is_locked)
#define In_Nets(c) ((c).in_net)
#define In_Part(c) ((c).in_part)

#endif