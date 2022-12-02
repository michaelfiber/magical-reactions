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
void getWorld(int worldSeed)
{
	srand(worldSeed);

	for (int i = 0; i < 10; i++)
	{
		TraceLog(LOG_INFO, TextFormat("%i", rand() % 100));
	}
}

void GenerateWorldImages(int worldSeed, Image *heightMap, Image *color)
{
	srand(worldSeed);

	int mountainCount = rand() % 5 + 5;

	TraceLog(LOG_INFO, TextFormat("Mountain count: %i", mountainCount));

	MountainLocation *mountains = (MountainLocation *)MemAlloc(sizeof(MountainLocation) * mountainCount);

	for (int i = 0; i < mountainCount; i++)
	{
		mountains[i].x = rand() % 256;
		mountains[i].y = rand() % 256;
		mountains[i].height = (rand() % 256);
		TraceLog(LOG_INFO, TextFormat("%ix%i@%i", mountains[i].x, mountains[i].y, mountains[i].height));
	}

	unsigned char *pixels = (unsigned char *)MemAlloc(sizeof(unsigned char) * 256 * 256);

	int minHeight = 256;
	int maxHeight = 0;
	
	for (int i = 0; i < 256 * 256; i++)
	{
		int y = i / 256;
		int x = i % 256;
		int height = 0;

		for (int j = 0; j < mountainCount; j++)
		{
			int distance = sqrt(pow(abs(mountains[j].x - x), 2) + pow(abs(mountains[j].y - y), 2) + pow(mountains[j].height, 2));
			int newHeight = (365 - distance) / 365.0 * (mountains[j].height);
			
			if (newHeight > height) {
				height = newHeight;
			}
		}

		if (height < minHeight)
			minHeight = height;

		pixels[i] = height + ((rand() % 10) - 10);
	}

	float cutoff = minHeight + ((256 - minHeight) * 0.05);

	Color *colorData = (Color *)MemAlloc(sizeof(Color) * 256 * 256);

	for (int i = 0; i < 256 * 256; i++)
	{
		pixels[i] = (pixels[i] - cutoff) / (256 - cutoff) * 256;
		int biome = GetBiome(pixels[i]);
		switch (GetBiome(pixels[i]))
		{
		case BIOME_WATER:
			colorData[i] = BLUE;
			break;
		case BIOME_PEAK:
			colorData[i] = WHITE;
			break;
		case BIOME_ROCKY:
			colorData[i] = BROWN;
			break;
		case BIOME_GRASS:
			colorData[i] = GREEN;
			break;
		case BIOME_SNOW:
			colorData[i] = LIGHTGRAY;
			break;
		case BIOME_SAND:
			colorData[i] = YELLOW;
			break;
		}
	}

	color->data = colorData;
	color->width = 256;
	color->height = 256;
	color->format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
	color->mipmaps = 1;

	heightMap->data = pixels;
	heightMap->width = 256;
	heightMap->height = 256;
	heightMap->format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
	heightMap->mipmaps = 1;
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

void GetLocalMap(Location worldLoc, int elevation)
{
	int biome = GetBiome(elevation);
}