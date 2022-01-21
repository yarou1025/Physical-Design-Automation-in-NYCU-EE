#ifndef NET_H
#define NET_H
#include <vector>

using namespace std;
typedef struct Net
{
    int in_A; // # of connected cells in part A
    int in_B;
    int lock_count;
    int is_cut;
    int cells;
    vector<int> in_cell;
} Net;

#define A(n) ((n).in_A)
#define B(n) ((n).in_B)
#define Locks_In(n) ((n).lock_count)
#define Links(n) ((n.in_cell))
#define N(n) ((n).cells)

#endif