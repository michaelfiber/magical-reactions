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

int *getWorld()
{
	int *world = (int *)MemAlloc(sizeof(int) * 256 * 256);

	for (int y = 0; y < 256; y++)
	{
		for (int x = 0; x < 256; x++)
		{
			world[y * 256 + x] = 0;
		}
	}

	return world;
}

#endif
