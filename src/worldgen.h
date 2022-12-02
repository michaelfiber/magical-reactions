#ifndef WORLDGEN_H
#define WORLDGEN_H

#include "raylib.h"

typedef struct
{
	int x;
	int y;
} Location;

typedef struct
{
	Location world;
	Location region;
	Location local;
} WorldThing;

void getWorld(int worldSeed);
Image genTiles(int worldSeed);

#endif
