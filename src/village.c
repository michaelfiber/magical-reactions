#include "stdlib.h"
#include "raylib.h"
#include "player.h"
#include "sprite.h"
#include "myrandom.h"

Texture2D campfireTexture;

bool hasVillage = false;

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

void InitVillage()
{
	ResetSprites();

	// checks if village exists here and runs srand for this village.
	if (!HasVillageDestructive(player.world))
		return;

	campfireTexture = LoadTexture("resources/campfire_32x32.png");

	TraceLog(LOG_INFO, TextFormat("campfire texture width %i", campfireTexture.width));
	int campfire = AddSprite(campfireTexture, (Vector2){Random(0, 255) * 32, Random(0, 255) * 32}, 0.3f);

	TraceLog(LOG_INFO, TextFormat("Loaded campfire as sprite #%i", campfire));
	TraceLog(LOG_INFO, TextFormat("Campfire is at %f %f", sprites[campfire].location.x, sprites[campfire].location.y));
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
}

void UnloadVillage()
{
	UnloadTexture(campfireTexture);
}