#ifndef _TILES_H
#define _TILES_H

typedef struct
{
    int x;
    int y;
} Point;

typedef struct tile
{
    struct tile *lb;
    struct tile *bl;
    struct tile *tr;
    struct tile *rt;

    int is_block;
    int BlockNumber;

    Point l;
    Point r;
} Tile;

typedef struct _linkedTile
{
    Tile t;
    struct _linkedTile *neighbor;
} LinkedTile;

#define P_RX(t) ((t)->r.x)
#define P_RY(t) ((t)->r.y)
#define P_LX(t) ((t)->l.x)
#define P_LY(t) ((t)->l.y)
#define LB(t) ((t)->lb)
#define BL(t) ((t)->bl)
#define TR(t) ((t)->tr)
#define RT(t) ((t)->rt)

#endif