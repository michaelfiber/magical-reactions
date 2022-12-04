#include "stdlib.h"
#include "raylib.h"
#include "player.h"
#include "sprite.h"
#include "myrandom.h"
#include "village.h"

Texture2D campfireTexture;
Texture2D villageTexture;

typedef struct
{
	Rectangle src;
	int gridWidth;
	int gridHeight;
} HouseType;

HouseType houseTypes[] = {
	{.src = (Rectangle){0, 688, 96, 102},
	 .gridWidth = 3,
	 .gridHeight = 3},
	{.src = (Rectangle){96, 688, 96, 102},
	 .gridWidth = 3,
	 .gridHeight = 3},
	{.src = (Rectangle){192, 672, 128, 118},
	 .gridWidth = 4,
	 .gridHeight = 3},
	{.src = (Rectangle){320, 672, 160, 118},
	 .gridWidth = 5,
	 .gridHeight = 3},
	{.src = (Rectangle){0, 688, 96, 102},
	 .gridWidth = 3,
	 .gridHeight = 3},
	{.src = (Rectangle){96, 688, 96, 102},
	 .gridWidth = 3,
	 .gridHeight = 3},
	{.src = (Rectangle){192, 672, 128, 118},
	 .gridWidth = 4,
	 .gridHeight = 3},
	{.src = (Rectangle){320, 672, 160, 118},
	 .gridWidth = 5,
	 .gridHeight = 3},
	{.src = (Rectangle){0, 688, 96, 102},
	 .gridWidth = 3,
	 .gridHeight = 3},
	{.src = (Rectangle){96, 688, 96, 102},
	 .gridWidth = 3,
	 .gridHeight = 3},
	{.src = (Rectangle){192, 672, 128, 118},
	 .gridWidth = 4,
	 .gridHeight = 3},
	{.src = (Rectangle){320, 672, 160, 118},
	 .gridWidth = 5,
	 .gridHeight = 3}};

int houseTypeCount = 12;

int houseCount = 4;

bool hasVillage = false;

typedef struct
{
	bool isActive;
	Location location;
	int houseType
} House;

bool VillagePassableGrid[256 * 256] = {0};
House houses[32] = {0};
Location center = {0};
int VillageDisposition = 0;

/**
 * @brief RUNS SRAND - Checks if a location has a village. There is a 1/100 chance that a grass biome will have a village.
 *
 * @param location Location to check
 * @return true if a village is there
 * @return false if there is no village
 */
bool HasVillageDestructive(Location location)
{
	SeedRandom(location.y * 256 + location.x);

	hasVillage = GetBiomeAtWorldLocation(location) == BIOME_GRASS && Random(0, 100) < 2;

	return hasVillage;
}

void GenerateHouses()
{

	for (int i = 0; i < 32; i++)
	{
		houses[i].isActive = false;
	}

	int houseCount = Random(1, 14);

	if (houseCount == 1)
	{
		VillageDisposition = DISPO_ALONE;
	}
	else
	{
		VillageDisposition = Random(1, DISPO_COUNT - 1);
	}

	int x = center.x - ((houseCount > 7 ? houseCount / 2 : houseCount) * 4) / 2;
	int y = center.y - 3;

	for (int i = 0; i < houseCount; i++)
	{
		houses[i].houseType = Random(0, houseTypeCount - 1);
		houses[i].isActive = true;
		houses[i].location.x = x;
		houses[i].location.y = y;
		x += houseTypes[houses[i].houseType].gridWidth + 2;

		if (i > 0 && i % 6 == 0)
		{
			x = center.x - ((houseCount > 7 ? houseCount / 2 : houseCount) * 4) / 2;
			y += 5;
		}
	}
}

void UpdatePassableGrid()
{
	for (int i = 0; i < 256 * 256; i++)
	{
		VillagePassableGrid[i] = true;
	}

	for (int i = 0; i < 32; i++)
	{
		if (houses[i].isActive)
		{
			for (int x = 0; x < houseTypes[houses[i].houseType].gridWidth; x++)
			{
				for (int y = 0; y < houseTypes[houses[i].houseType].gridHeight; y++)
				{
					int index = (houses[i].location.y + y) * 256 + houses[i].location.x + x;
					VillagePassableGrid[index] = false;
				}
			}
		}
	}
}

void InitVillage()
{
	ResetSprites();

	// Update houses 4-7 to be srced 256 pixels lower than 0-3 instead of typing out all new src recs above.
	for (int i = 4; i < 8; i++)
	{
		houseTypes[i].src.y += 256;
	}
	// Alsp update 8-11 to be srced 512
	for (int i = 8; i < 12; i++) {
		houseTypes[i].src.y += 512;
	}

	// checks if village exists here and runs srand for this village.
	if (!HasVillageDestructive(player.world))
		return;

	campfireTexture = LoadTexture("resources/campfire_32x32.png");

	TraceLog(LOG_INFO, TextFormat("campfire texture width %i", campfireTexture.width));
	center.x = Random(50, 205);
	center.y = Random(50, 205);

	int campfire = AddSprite(campfireTexture, (Vector2){center.x * 32, center.y * 32}, 0.3f);

	TraceLog(LOG_INFO, TextFormat("Loaded campfire as sprite #%i", campfire));
	TraceLog(LOG_INFO, TextFormat("Campfire is at %f %f", sprites[campfire].location.x, sprites[campfire].location.y));

	for (int i = 0; i < 256 * 256; i++)
	{
		VillagePassableGrid[i] = true;
	}

	villageTexture = LoadTexture("resources/Serene_Village_32x32.png");

	GenerateHouses();
	UpdatePassableGrid();
}

void UpdateVillage()
{
	UpdateSprites();
}

void DrawVillage()
{
	DrawSprites();
	if (hasVillage)
		DrawText("There's a village nearby", player.pos.x, player.pos.y, GetFontDefault().baseSize, BLACK);

	for (int i = 0; i < 32; i++)
	{
		if (houses[i].isActive)
		{
			Rectangle src = houseTypes[houses[i].houseType].src;
			DrawTexturePro(villageTexture, src, (Rectangle){houses[i].location.x * 32, houses[i].location.y * 32 + houseTypes[houses[i].houseType].gridHeight * 32 - src.height, src.width, src.height}, (Vector2){0, 0}, 0.0f, WHITE);
		}
	}
}

void UnloadVillage()
{
	UnloadTexture(campfireTexture);
	UnloadTexture(villageTexture);
}