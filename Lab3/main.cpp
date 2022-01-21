
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <cmath>
#include <stack>
#include <algorithm>
#include <ctime>
#include "Floorplan.cpp"

using namespace std;

int main(int argc, char *argv[])
{
    Floorplan(stod(argv[1]), argv[2], argv[3], argv[4]);
    // PrintAll();
}

// ./Lab3 0.05 ./ami33/ami33.block ami33/ami33.nets output.rpt