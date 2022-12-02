#include "stdlib.h"
#include "math.h"
#include "worldgen.h"

typedef struct
{
	int x;
	int y;
	int height;
} MountainLocation;

/*
	world:
		ocean
		land
		mountain?

	region:
		river?
*/

Color GetBiomeColor(int biome)
{
	Color color = BLUE;

	switch (biome)
	{
	case BIOME_WATER:
		color = BLUE;
		break;
	case BIOME_PEAK:
		color = WHITE;
		break;
	case BIOME_ROCKY:
		color = BROWN;
		break;
	case BIOME_GRASS:
		color = GREEN;
		break;
	case BIOME_SNOW:
		color = LIGHTGRAY;
		break;
	case BIOME_SAND:
		color = YELLOW;
		break;
	}

	return color;
}

int heights[256 * 256] = {0};

Image GenerateWorldImage(Image heightMap)
{
	Color *colorData = (Color *)MemAlloc(sizeof(Color) * 256 * 256);

	unsigned char *pixels = heightMap.data;

	for (int i = 0; i < 256 * 256; i++)
	{
		int biome = GetBiome(pixels[i]);
		heights[i] = pixels[i];
		colorData[i] = GetBiomeColor(biome);
	}

	Image worldImage = {.data = colorData,
						.width = 256,
						.height = 256,
						.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
						.mipmaps = 1};

	return worldImage;
}

int GetBiome(int elevation)
{
	int biome = BIOME_WATER;
	if (elevation < 5)
	{
		biome = BIOME_WATER;
	}
	else if (elevation < 40)
	{
		biome = BIOME_SAND;
	}
	else if (elevation < 150)
	{
		biome = BIOME_GRASS;
	}
	else if (elevation < 200)
	{
		biome = BIOME_ROCKY;
	}
	else if (elevation < 218)
	{
		biome = BIOME_SNOW;
	}
	else
	{
		biome = BIOME_PEAK;
	}
	return biome;
}

/**
 * @brief Get the Local Map object - 256x256 grid of 32x32 tiles that represent the gameplay map.
 *
 * @param worldLoc
 * @param pos
 * @param elevation
 */
void FillLocalMap(Location worldLoc, Vector2 pos, LocalWorldNode *localMap)
{
	int elevation = heights[worldLoc.y * 256 + worldLoc.x];

	srand(worldLoc.y * 256 + worldLoc.x);
	TraceLog(LOG_INFO, TextFormat("Elevation %i", elevation));

	int biome = GetBiome(elevation);
	for (int i = 0; i < 256 * 256; i++)
	{
		localMap[i].passable = (biome != BIOME_WATER);
		localMap[i].tile = rand() % 8;
	}
}

int GetElevationAtWorldLoc(Location worldLoc)
{
	return heights[worldLoc.y * 256 + worldLoc.x];
}

int GetBiomeAtWorldLocation(Location worldLoc)
{
	return GetBiome(GetElevationAtWorldLoc(worldLoc));
}

Image GenerateTileMap()
{
	int width = 32;
	int height = 32 * 6 * 8;

	Color *pixels = (Color *)MemAlloc(sizeof(Color) * width * height);

	int currentPixel = 0;

	for (int biome = 0; biome < 6; biome++)
	{
		Vector3 hsv = {0};
		for (int currentTile = 0; currentTile < 8; currentTile++)
		{
			hsv = ColorToHSV(GetBiomeColor(biome));
			hsv.x += (rand() % 20) - 10;

			for (int i = 0; i < 32 * 32; i++)
			{
				float z = hsv.z * ((rand() % 20) + 80) / 100.0;
				pixels[currentPixel] = ColorFromHSV(hsv.x, hsv.y, z);
				currentPixel++;
			}
		}
	}

	Image img = {
		.data = pixels,
		.width = width,
		.height = height,
		.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
		.mipmaps = 1};

	return img;
}