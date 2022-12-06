#include "stdlib.h"
#include "math.h"
#include "worldgen.h"
#include "village.h"
#include "player.h"
#include "myrandom.h"

#define FNL_IMPL 1
#include "FastNoiseLite.h"

typedef struct
{
	int x;
	int y;
	int height;
} MountainLocation;

int heights[256 * 256] = {0};

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

Image GenerateHeightMap(int worldSeed)
{
	unsigned char *data = (unsigned char *)MemAlloc(sizeof(unsigned char) * 256 * 256);
	fnl_state noise = fnlCreateState();
	noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
	noise.seed = worldSeed;

	int index = 0;
	int max = 0;

	for (int y = 0; y < 256; y++)
	{
		for (int x = 0; x < 256; x++)
		{
			int dx = abs(x - 128);
			int dy = abs(y - 128);
			float distance = sqrt((dx * dx) + (dy * dy));
			data[index++] = (fnlGetNoise2D(&noise, x, y) + 1) * 128 * pow((185 - distance) / 185, 1.1);
			if (data[index - 1] > max)
				max = data[index - 1];
		}
	}

	TraceLog(LOG_INFO, TextFormat("max: %i", max));

	float rate = (max * 1.0f) / 255;
	for (int i = 0; i < 256 * 256; i++) {
		data[i] = data[i] / rate;
	}

	Image heightmap = {
		.data = data,
		.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE,
		.width = 256,
		.height = 256,
		.mipmaps = 1};

	return heightmap;
}

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
	if (elevation < 75)
	{
		biome = BIOME_WATER;
	}
	else if (elevation < 90)
	{
		biome = BIOME_SAND;
	}
	else if (elevation < 170)
	{
		biome = BIOME_GRASS;
	}
	else if (elevation < 225)
	{
		biome = BIOME_ROCKY;
	}
	else if (elevation < 250)
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
 * @param elevation
 */
void FillLocalMap(LocalWorldNode *localMap)
{
	Location worldLoc = player.world;
	int elevation = heights[worldLoc.y * 256 + worldLoc.x];

	SeedRandom(worldLoc.y * 256 + worldLoc.x);
	TraceLog(LOG_INFO, TextFormat("Elevation %i", elevation));

	int biome = GetBiome(elevation);
	for (int i = 0; i < 256 * 256; i++)
	{
		localMap[i].passable = (biome != BIOME_WATER);
		localMap[i].tile = Random(0, 7);
		localMap[i].item = Random(0, 100);
		if (localMap[i].item > 7)
			localMap[i].item = -1;

		if (localMap[i].item > -1 && localMap[i].item < 4)
			localMap[i].passable = false;
	}

	InitVillage();
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
	int height = 32 * BIOME_COUNT * 8;

	Color *pixels = (Color *)MemAlloc(sizeof(Color) * width * height);

	int currentPixel = 0;

	for (int biome = 0; biome < 6; biome++)
	{
		Vector3 hsv = {0};
		for (int currentTile = 0; currentTile < 8; currentTile++)
		{
			hsv = ColorToHSV(GetBiomeColor(biome));
			hsv.x += Random(-10, 10);

			for (int i = 0; i < 32 * 32; i++)
			{
				float z = hsv.z * Random(80, 100) / 100.0;
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

Texture2D GenerateItemMap()
{
	Texture2D items = LoadTexture("resources/item-atlas.png");
	return items;
}