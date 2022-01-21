#ifndef IO_FILE
#define IO_FILE
#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include "tile.h"
#include "tile.cpp"

using namespace std;

void WriteResult(vector<Tile *> ResTile, vector<Point> ResPoint, int size);
void read_file(string filename);

int TotalTile();

void read_file(string filename)
{
    // cout << filename << endl;
    fstream infile;
    infile.open(filename, ios::in);
    string line, word;
    Point l, r;
    vector<Point> ResPoint;
    vector<Tile *> ResTile;

    int size = 0;
    Tile *graph = new Tile;

    if (infile.is_open())
    {
        getline(infile, line);
        stringstream ss(line);
        l.x = 0;
        l.y = 0;
        ss >> r.x >> r.y;
        Point p = {r.x, r.y};
        ResPoint.push_back(p);
        size = size + 1;
        InitTile(l, r, graph);

        while (getline(infile, line))
        {
            ss.clear();
            ss.str(line);
            ss >> word;

            if (word == "P")
            {
                cout << "======== IF ========" << endl;
                ss >> l.x >> l.y;
                p.x = l.x;
                p.y = l.y;
                ResPoint.push_back(p);
                cout << "======== End If ========" << endl;
            }
            else
            { // Add Tile
                // cout << "Number: " << word << endl; // << ss << ss << ss << ss;
                cout << "======== Else ========" << endl;
                int num = stoi(word);
                ss >> l.x >> l.y >> r.x >> r.y;
                r.x = r.x + l.x;
                r.y = r.y + l.y;
                // cout << "N:" << num << endl;
                Tile *ptr = AddTile(l, r, graph, size);
                // PrintTile(ptr);

                ptr->is_block = 1;
                ptr->BlockNumber = num;

                ResTile.push_back(ptr);
                cout << "======== End Else ========" << endl;
            }
        }
        // PrintTile(graph);
        infile.close();
        // Find neighbor
        cout << "======== Before Sort ========" << endl;
        sort(ResTile.begin(), ResTile.end(), cmp);

        cout << "======== After Sort ========" << endl;

        // Call output_write
        WriteResult(ResTile, ResPoint, size);
    }
}

void WriteResult(vector<Tile *> ResTile, vector<Point> ResPoint, int size)
{
    fstream outfile;
    outfile.open("output.txt", ios::out);
    int B, S;
    Tile *tmp;
    outfile << size << endl;
    for (int i = 0; i < ResTile.size(); i++)
    {
        outfile << ResTile[i]->BlockNumber << " ";
        B = 0;
        S = 0;
        // PrintTile(ResTile[i]);
        //  right
        for (tmp = TR(ResTile[i]); tmp != NULL && P_RY(tmp) > P_LY(ResTile[i]); tmp = LB(tmp))
        {
            // cout << "right: ";
            // PrintTile(tmp);
            if (tmp->is_block == 1)
                B = B + 1;
            else
                S = S + 1;
        }

        // bottom
        for (tmp = LB(ResTile[i]); tmp != NULL && P_LX(tmp) < P_RX(ResTile[i]); tmp = TR(tmp))
        {
            // cout << "bottom: ";
            if (tmp->is_block == 1)
                B = B + 1;
            else
                S = S + 1;
        }

        // left
        for (tmp = BL(ResTile[i]); tmp != NULL && P_LY(tmp) < P_RY(ResTile[i]); tmp = RT(tmp))
        {
            // cout << "left: ";
            if (tmp->is_block == 1)
                B = B + 1;
            else
                S = S + 1;
        }

        // top
        for (tmp = RT(ResTile[i]); tmp != NULL && P_RX(tmp) > P_LX(ResTile[i]); tmp = BL(tmp))
        {
            // cout << "top: ";
            if (tmp->is_block == 1)
                B = B + 1;
            else
                S = S + 1;
        }
        outfile << B << " " << S << endl;
    }
    for (int i = 1; i < ResPoint.size(); i++)
    {
        // cout << ResPoint[i].x << ", " << ResPoint[i].y << endl;
        Tile *tmp = FindPoint(ResPoint[i], ResTile[0]);
        outfile << P_LX(tmp) << " " << P_LY(tmp) << endl;
    }
    // outfile << "OWO" << endl;

    outfile.close();
}

#endif
