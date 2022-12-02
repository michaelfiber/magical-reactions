#ifndef WORLDGEN_H
#define WORLDGEN_H

#include "raylib.h"

#define BIOME_WATER 0
#define BIOME_SAND 	1
#define BIOME_GRASS 2
#define BIOME_ROCKY 3
#define BIOME_SNOW 	4
#define BIOME_PEAK 	5

#define BIOME_COUNT 6

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

int GetBiome(int elevation);
Color GetBiomeColor(int biome);
Image GenerateTileMap();
Image GenerateWorldImage(Image heightMap);

#endif
