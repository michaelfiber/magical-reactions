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

Image genTiles(int worldSeed)
{
	srand(worldSeed);

	int mountainCount = rand() % 5 + 5;

	TraceLog(LOG_INFO, TextFormat("Mountain count: %i", mountainCount));

	MountainLocation *mountains = (MountainLocation *)MemAlloc(sizeof(MountainLocation) * mountainCount);

	for (int i = 0; i < mountainCount; i++)
	{
		mountains[i].x = rand() % 256;
		mountains[i].y = rand() % 256;
		mountains[i].height = (rand() % 128) + 128;
		TraceLog(LOG_INFO, TextFormat("%ix%i@%i", mountains[i].x, mountains[i].y, mountains[i].height));
	}

	unsigned char *pixels = (unsigned char *)MemAlloc(sizeof(unsigned char) * 256 * 256);

	int minHeight = 256;

	for (int i = 0; i < 256 * 256; i++)
	{
		int y = i / 256;
		int x = i % 256;
		int height = 0;

		for (int j = 0; j < mountainCount; j++)
		{
			int distance = sqrt(pow(abs(mountains[j].x - x), 2) + pow(abs(mountains[j].y - y), 2));
			if (height < 256 - distance)
			{
				height = 256 - distance;
			}
		}

		if (height < minHeight) minHeight = height;

		pixels[i] = height + ((rand() % 10) - 10);
	}

	float cutoff = minHeight + ((256 - minHeight) * 0.4);
	for (int i = 0; i < 256 * 256; i++) {
		pixels[i] = (pixels[i] - cutoff) / (256 - cutoff) * 256;
	}

	Image img = {.data = pixels, .width = 256, .height = 256, .format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE, .mipmaps = 1};

	return img;
}