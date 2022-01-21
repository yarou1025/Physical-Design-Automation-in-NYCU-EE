#ifndef TILE_CPP
#define TILE_CPP
#include <algorithm>
#include "tile.h"
#include <iostream>
using namespace std;

Tile *VSplit_R(Tile *tile, int x);
Tile *HSplit_T(Tile *tile, int y);
Tile *VSplit_L(Tile *tile, int x);
Tile *HSplit_B(Tile *tile, int y);
Tile *FindPoint(Point p, Tile *start);
Tile *AddTile(Point l, Point r, Tile *start, int &size);
Tile *VMerge(Tile *t1, Tile *t2);
Tile *HMerge(Tile *t1, Tile *t2);

void PrintTile(Tile *tmp);
void InitTile(Point l, Point r, Tile *t);
void PrintPoint(Tile *tmp);

bool cmp(const Tile *t1, const Tile *t2);

// newptr is right part of the original
Tile *VSplit_R(Tile *tile, int x)
{
    Tile *newptr = new Tile;
    newptr->is_block = 0;
    Tile *tmp;
    P_LX(newptr) = x;
    P_LY(newptr) = P_LY(tile);
    P_RX(newptr) = P_RX(tile);
    P_RY(newptr) = P_RY(tile);
    P_RX(tile) = x;

    BL(newptr) = tile;
    TR(newptr) = TR(tile);
    RT(newptr) = RT(tile);

    // right check
    for (tmp = TR(tile); tmp != NULL && BL(tmp) == tile; tmp = LB(tmp))
        BL(tmp) = newptr;
    TR(tile) = newptr;

    // top check
    for (tmp = RT(tile); tmp != NULL && P_LX(tmp) >= x; tmp = BL(tmp))
        LB(tmp) = newptr;
    RT(tile) = tmp;

    // bottom check
    for (tmp = LB(tile); tmp != NULL && P_RX(tmp) <= x; tmp = TR(tmp))
        ;

    LB(newptr) = tmp;
    while (tmp != NULL && RT(tmp) == tile)
    {
        RT(tmp) = newptr;
        tmp = TR(tmp);
    }

    return newptr;
}

// newptr is upper part of the original
Tile *HSplit_T(Tile *tile, int y)
{
    Tile *newptr = new Tile;
    newptr->is_block = 0;
    Tile *tmp;
    P_LY(newptr) = y;
    P_LX(newptr) = P_LX(tile);
    P_RY(newptr) = P_RY(tile);
    P_RX(newptr) = P_RX(tile);
    P_RY(tile) = y;

    LB(newptr) = tile;
    RT(newptr) = RT(tile);
    TR(newptr) = TR(tile);

    // top check
    for (tmp = RT(tile); tmp != NULL && LB(tmp) == tile; tmp = BL(tmp))
        LB(tmp) = newptr;
    RT(tile) = newptr;

    // right check
    for (tmp = TR(tile); tmp != NULL && P_LY(tmp) >= y; tmp = LB(tmp))
        BL(tmp) = newptr;
    TR(tile) = tmp;

    // left check
    for (tmp = BL(tile); tmp != NULL && P_RY(tmp) <= y; tmp = RT(tmp))
        ;

    BL(newptr) = tmp;
    while (tmp != NULL && TR(tmp) == tile)
    {
        TR(tmp) = newptr;
        tmp = RT(tmp);
    }

    return newptr;
}

// newptr is left part of the original
Tile *VSplit_L(Tile *tile, int x)
{
    Tile *newptr = new Tile;
    newptr->is_block = 0;
    Tile *tmp;
    P_RX(newptr) = x;
    P_LX(newptr) = P_LX(tile);
    P_LY(newptr) = P_LY(tile);
    P_RY(newptr) = P_RY(tile);
    P_LX(tile) = x;

    LB(newptr) = LB(tile);
    BL(newptr) = BL(tile);
    TR(newptr) = tile;
    BL(tile) = newptr;

    // left check
    for (tmp = BL(newptr); tmp != NULL && TR(tmp) == tile; tmp = RT(tmp))
        TR(tmp) = newptr;

    // top check
    for (tmp = RT(tile); tmp != NULL && P_LX(tmp) >= x; tmp = BL(tmp))
        ;
    RT(newptr) = tmp;
    while (tmp != NULL && LB(tmp) == tile)
    {
        LB(tmp) = newptr;
        tmp = BL(tmp);
    }

    // bottom check
    for (tmp = LB(tile); tmp != NULL && P_RX(tmp) <= x; tmp = TR(tmp))
        RT(tmp) = newptr;
    LB(tile) = tmp;

    return newptr;
}

// newptr is lower part of the original
Tile *HSplit_B(Tile *tile, int y)
{
    Tile *newptr = new Tile;
    newptr->is_block = 0;
    Tile *tmp;
    P_RY(newptr) = y;
    P_LX(newptr) = P_LX(tile);
    P_LY(newptr) = P_LY(tile);
    P_RX(newptr) = P_RX(tile);
    P_LY(tile) = y;

    RT(newptr) = tile;
    LB(newptr) = LB(tile);
    BL(newptr) = BL(tile);
    LB(tile) = newptr;

    // bottom check
    for (tmp = LB(newptr); tmp != NULL && RT(tmp) == tile; tmp = TR(tmp))
        RT(tmp) = newptr;

    // right check
    for (tmp = TR(tile); tmp != NULL && P_LY(tmp) >= y; tmp = LB(tmp))
        ;

    TR(newptr) = tmp;
    while (tmp != NULL && BL(tmp) == tile)
    {
        BL(tmp) = newptr;
        tmp = LB(tmp);
    }
    for (tmp = BL(tile); tmp != NULL && P_RY(tmp) <= y; tmp = RT(tmp))
        TR(tmp) = newptr;
    BL(tile) = tmp;

    return newptr;
}

Tile *HMerge(Tile *t1, Tile *t2) // t1 | t2
{
    Tile *tmp;
    if (P_LX(t1) > P_LX(t2))
    {
        HMerge(t2, t1);
        return t2;
    }

    // top check
    for (tmp = RT(t2); tmp != NULL && LB(tmp) == t2; tmp = BL(tmp))
        LB(tmp) = t1;

    // bottom check
    for (tmp = LB(t2); tmp != NULL && RT(tmp) == t2; tmp = TR(tmp))
        RT(tmp) = t1;

    // right check
    for (tmp = TR(t2); tmp != NULL && BL(tmp) == t2; tmp = LB(tmp))
        BL(tmp) = t1;
    TR(t1) = TR(t2);
    RT(t1) = RT(t2);

    P_RX(t1) = P_RX(t2);
    P_RY(t1) = P_RY(t2);

    delete t2;
    return t1;
}

// t2
// --
// t1
// merge t2 to t1
Tile *VMerge(Tile *t1, Tile *t2)
{
    // cout << "Before: ";
    // PrintTile(t1);
    Tile *tmp;
    if (P_LY(t1) > P_LY(t2))
    {
        VMerge(t2, t1);
        return t2;
    }

    // right check
    for (tmp = TR(t2); tmp != NULL && BL(tmp) == t2; tmp = LB(tmp))
        BL(tmp) = t1;

    // left
    for (tmp = BL(t2); tmp != NULL && TR(tmp) == t2; tmp = RT(tmp))
        TR(tmp) = t1;

    // top
    for (tmp = RT(t2); tmp != NULL && LB(tmp) == t2; tmp = BL(tmp))
        LB(tmp) = t1;

    RT(t1) = RT(t2);
    TR(t1) = TR(t2);

    P_RX(t1) = P_RX(t2);
    P_RY(t1) = P_RY(t2);

    delete t2;
    // cout << "After: ";
    // PrintTile(t1);
    return t1;
}

Tile *FindPoint(Point p, Tile *tmp)
{
    if (p.y < P_LY(tmp))
    {
        while (p.y < P_LY(tmp))
            tmp = LB(tmp);
    }
    else
    {
        while (p.y >= P_RY(tmp))
            tmp = RT(tmp);
    }
    if (p.x < P_LX(tmp))
    {
        do
        {
            while (p.x < P_LX(tmp))
                tmp = BL(tmp);

            if (p.y < P_RY(tmp))
                return tmp;

            while (p.y >= P_RY(tmp))
                tmp = RT(tmp);

        } while (p.x < P_LX(tmp));
    }
    else
    {

        while (p.x >= P_RX(tmp))
        {
            do
                tmp = TR(tmp);
            while (p.x >= P_RX(tmp));

            if (p.y >= P_LY(tmp))
                return tmp;

            do
                tmp = LB(tmp);
            while (p.y < P_LY(tmp));
        }
    }
    return tmp;
}

Tile *AddTile(Point l, Point r, Tile *start, int &size)
{
    int Append_Tiles = 0;
    Tile *top = FindPoint(r, start);
    Tile *top_b;

    if (P_LY(top) == r.y)
    {
        // cout << "equal height" << endl;
        for (top_b = LB(top); top_b != NULL; top_b = TR(top_b))
        {
            if (P_RX(top_b) > l.x)
                break;
        }
    }
    else
    {
        top_b = HSplit_B(top, r.y);
        Append_Tiles++;
    }

    // cout << "Top: " << endl;
    // PrintTile(top);
    // PrintTile(top_b);

    Tile *bottom = FindPoint(l, start);
    Tile *bottom_b;
    if (P_LY(bottom) == l.y)
    {
        for (bottom_b = LB(bottom); bottom_b != NULL; bottom_b = TR(bottom_b))
        {
            if (P_RX(bottom_b) > l.x)
                break;
        }
        // bottom_b = bottom;
    }
    else
    {
        bottom_b = HSplit_B(bottom, l.y);
        Append_Tiles++;
    }
    // cout << "Bottom: " << endl;
    // PrintTile(bottom);
    // PrintTile(bottom_b);

    // walk down
    int i = 0;
    while (1)
    {
        // cout << "I " << i << endl;
        // PrintTile(top_b);
        Tile *left;
        if (P_LX(top_b) == l.x)
        {
            left = top_b;
        }
        else
        {
            left = VSplit_L(top_b, l.x);
            Append_Tiles++;
        }
        // PrintTile(top_b);
        Tile *right;
        if (P_RX(top_b) == r.x)
        {
            right = top_b;
        }
        else
        {
            right = VSplit_R(top_b, r.x);
            Append_Tiles++;
        }
        // PrintTile(right);

        if (P_LX(left) == P_LX(RT(left)) && P_RX(left) == P_RX(RT(left)))
        {
            // cout << "in merge" << endl;
            // PrintTile(left);
            VMerge(left, RT(left));
            Append_Tiles--;
        }

        if (P_RX(right) == P_RX(RT(right)) && P_LX(right) == P_LX(RT(right)))
        {
            // cout << "in merge" << endl;
            // PrintTile(right);
            VMerge(right, RT(right));
            Append_Tiles--;
        }

        if (i != 0)
        {
            VMerge(top_b, RT(top_b));
            Append_Tiles--;
        }
        // cout << "top_b" << endl;
        // PrintTile(top_b);
        if (LB(top_b) == NULL || LB(top_b) == bottom_b)
        {
            // cout << "Stop" << endl;
            break;
        }

        else
            top_b = LB(top_b);

        i = 1;
    }
    size = size + Append_Tiles;
    return top_b;
}

void PrintTile(Tile *tmp)
{
    if (tmp == NULL)
    {
        cout << "NULL" << endl;
        return;
    }

    else
        PrintPoint(tmp);
    if (BL(tmp) != NULL)
    {
        cout << "(BL) ";
        PrintPoint(BL(tmp));
    }
    if (LB(tmp) != NULL)
    {
        cout << "(LB) ";
        PrintPoint(LB(tmp));
    }
    if (RT(tmp) != NULL)
    {
        cout << "(RT) ";
        PrintPoint(RT(tmp));
    }
    if (TR(tmp) != NULL)
    {
        cout << "(TR) ";
        PrintPoint(TR(tmp));
    }

    cout << endl;
}

void InitTile(Point l, Point r, Tile *t)
{
    // cout << "Initialization..." << endl;

    P_LX(t) = l.x;
    P_RX(t) = r.x;
    P_LY(t) = l.y;
    P_RY(t) = r.y;
    TR(t) = NULL;
    RT(t) = NULL;
    BL(t) = NULL;
    LB(t) = NULL;
    t->is_block = 0;
    // PrintTile(t);
}

void PrintPoint(Tile *tmp)
{
    cout << "L=" << P_LX(tmp) << "," << P_LY(tmp);
    cout << " R=" << P_RX(tmp) << "," << P_RY(tmp);
    cout << endl;
}

bool cmp(const Tile *t1, const Tile *t2)
{
    return t1->BlockNumber < t2->BlockNumber;
}

#endif