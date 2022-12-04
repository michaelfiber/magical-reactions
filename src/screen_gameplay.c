/**********************************************************************************************
 *
 *   raylib - Advance Game template
 *
 *   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
 *
 *   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
 *
 *   This software is provided "as-is", without any express or implied warranty. In no event
 *   will the authors be held liable for any damages arising from the use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose, including commercial
 *   applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not claim that you
 *     wrote the original software. If you use this software in a product, an acknowledgment
 *     in the product documentation would be appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *     as being the original software.
 *
 *     3. This notice may not be removed or altered from any source distribution.
 *
 **********************************************************************************************/

#include "stdlib.h"
#include "math.h"
#include "raylib.h"
#include "screens.h"
#include "fontstyle.h"
#include "worldgen.h"
#include "village.h"
#include "player.h"
#include "myrandom.h"

#define MODE_WORLD 0
#define MODE_LOCAL 1

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Defines the player object exposed in player.h
Player player = {0};

Image heightMap = {0};
Image worldImg = {0};
Image tileMapImage = {0};

Texture2D worldTexture = {0};
Texture2D tileMap = {0};
Texture2D itemMap = {0};
Texture2D villageMarkers = {0};

LocalWorldNode localNodes[256 * 256] = {0};

/**
 * @brief Used during InitGameplayScreen to randomize worldwide assets.
 */
static int worldSeed = 12345;

static int mode = MODE_WORLD;

Camera2D camera = {0};

static bool drawTopNeighbor = false;
static bool drawRightNeighbor = false;
static bool drawBottomNeighbor = false;
static bool drawLeftNeighbor = false;

Texture2D biomeTexture = {0};
Texture2D grassTexture = {0};

/**
 * @brief RUNS SRAND
 *
 * @return Image
 */
Image GenerateVillageMarkers()
{
	Color *data = (Color *)MemAlloc(sizeof(Color) * 256 * 256);

	Color transparent = ColorAlpha(WHITE, 0.0f);

	// After this commands may rerun srand.
	Location l = {0, 0};

	for (int x = 0; x < 256; x++)
	{
		for (int y = 0; y < 256; y++)
		{
			l.x = x;
			l.y = y;
			data[y * 256 + x] = HasVillageDestructive(l) ? BLACK : transparent;
		}
	}

	return (Image){
		.data = data,
		.width = 256,
		.height = 256,
		.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
		.mipmaps = 1};
}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
	// TODO: Initialize GAMEPLAY screen variables here!
	framesCounter = 0;
	finishScreen = 0;

	FontStyle *small = (FontStyle *)(MemAlloc(sizeof(FontStyle)));
	small->color = MAROON;
	small->font = GetFontDefault();
	small->size = small->font.baseSize;
	small->spacing = 4;

	grassTexture = LoadTexture("resources/trees.png");
	biomeTexture = grassTexture;

	LoadFontStyle("gameplaysmall", small);
	SetCurrentFontStyle("gameplaysmall");

	SeedRandom(worldSeed);
	heightMap = LoadImage("resources/world-height.png");
	worldImg = GenerateWorldImage(heightMap);
	worldTexture = LoadTextureFromImage(worldImg);

	tileMapImage = GenerateTileMap();
	tileMap = LoadTextureFromImage(tileMapImage);

	itemMap = GenerateItemMap();

	player.world.x = 165;
	player.world.y = 128;

	player.pos.x = 128;
	player.pos.y = 3298;

	player.speed = 100.0f;

	mode = MODE_LOCAL;

	camera.zoom = 1.0f;
	camera.offset.x = 128;
	camera.offset.y = 128;

	// This will run SeedRandom tons of times.
	Image img = GenerateVillageMarkers();
	villageMarkers = LoadTextureFromImage(img);
	UnloadImage(img);

	// this also sets up the village and calls SeedRandom
	FillLocalMap(&localNodes);
}

void UpdateWorldLoc()
{
	bool worldLocChanged = false;

	if (player.pos.x < -32 && player.world.x > 0)
	{
		if (GetBiomeAtWorldLocation((Location){player.world.x - 1, player.world.y}) != BIOME_WATER)
		{
			player.pos.x = 32 * 256 - 16;
			player.world.x--;
			worldLocChanged = true;
		}
		else
		{
			player.pos.x = -32;
		}
	}

	if (player.pos.y < -32 && player.world.y > 0)
	{
		if (GetBiomeAtWorldLocation((Location){player.world.x, player.world.y - 1}) != BIOME_WATER)
		{
			player.pos.y = 32 * 256 - 16;
			player.world.y--;
			worldLocChanged = true;
		}
		else
		{
			player.pos.y = -32;
		}
	}

	if (player.pos.x > 32 * 256 + 16 && player.world.x < 256)
	{
		if (GetBiomeAtWorldLocation((Location){player.world.x + 1, player.world.y}) != BIOME_WATER)
		{
			player.pos.x = 0;
			player.world.x++;
			worldLocChanged = true;
		}
		else
		{
			player.pos.x = 32 * 256 + 16;
		}
	}

	if (player.pos.y > 32 * 256 + 16 && player.world.y < 256)
	{
		if (GetBiomeAtWorldLocation((Location){player.world.x, player.world.y + 1}) != BIOME_WATER)
		{
			player.pos.y = 0;
			player.world.y++;
			worldLocChanged = true;
		}
		else
		{
			player.pos.y = 32 * 256 + 16;
		}
	}

	if (worldLocChanged)
	{
		FillLocalMap(&localNodes);
	}
}

void UpdateGameInput()
{

	if (IsKeyPressed(KEY_SPACE))
	{
		mode++;
		if (mode > MODE_LOCAL)
			mode = MODE_WORLD;
	}

	if (mode == MODE_LOCAL)
	{
		Vector2 vel = {0};

		if (IsKeyDown(KEY_UP))
		{
			vel.y -= 1; // GetFrameTime() * player.speed;
		}

		if (IsKeyDown(KEY_DOWN))
		{
			vel.y += 1; // GetFrameTime() * player.speed;
		}

		if (IsKeyDown(KEY_LEFT))
		{
			vel.x -= 1; // GetFrameTime() * player.speed;
		}

		if (IsKeyDown(KEY_RIGHT))
		{
			vel.x += 1; // GetFrameTime() * player.speed;
		}

		if (fabs(vel.x) > 0 && fabs(vel.y) > 0)
		{
			vel.x = 0.7 * vel.x;
			vel.y = 0.7 * vel.y;
		}

		int localX = player.pos.x / 32;
		int localY = player.pos.y / 32;

		float xInc = vel.x * GetFrameTime() * player.speed;
		float yInc = vel.y * GetFrameTime() * player.speed;

		if (vel.x > 0)
		{

			Rectangle rec = {player.pos.x + xInc, player.pos.y, 32, 32};

			// check the top right
			if (!localNodes[localY * 256 + (localX + 1)].passable && CheckCollisionRecs(rec, (Rectangle){(localX + 1) * 32, localY * 32, 32, 32}))
			{
				xInc = 0;
				player.pos.x = localX * 32;
			}

			rec.x = player.pos.x + xInc;

			// check the bottom right
			if (!localNodes[(localY + 1) * 256 + (localX + 1)].passable && CheckCollisionRecs(rec, (Rectangle){(localX + 1) * 32, (localY + 1) * 32, 32, 32}))
			{
				xInc = 0;
				player.pos.x = localX * 32;
			}
		}
		else if (vel.x < 0)
		{
			Rectangle rec = {player.pos.x + xInc, player.pos.y + yInc, 32, 32};

			// check the top left
			if (!localNodes[localY * 256 + (localX - 1)].passable && CheckCollisionRecs(rec, (Rectangle){(localX - 1) * 32, localY * 32, 32, 32}))
			{
				xInc = 0;
				player.pos.x = localX * 32;
			}

			rec.x = player.pos.x + xInc;

			// check the bottom left
			if (!localNodes[(localY + 1) * 256 + (localX - 1)].passable && CheckCollisionRecs(rec, (Rectangle){(localX - 1) * 32, (localY + 1) * 32, 32, 32}))
			{
				xInc = 0;
				player.pos.x = localX * 32;
			}
		}

		if (vel.y > 0)
		{
			Rectangle rec = {player.pos.x, player.pos.y + yInc, 32, 32};
			// check bottom left
			if (!localNodes[(localY + 1) * 256 + localX].passable && CheckCollisionRecs(rec, (Rectangle){localX * 32, (localY + 1) * 32, 32, 32}))
			{
				yInc = 0;
				player.pos.y = localY * 32;
			}

			rec.y = player.pos.y + yInc;
			// check bottom right
			if (!localNodes[(localY + 1) * 256 + (localX + 1)].passable && CheckCollisionRecs(rec, (Rectangle){(localX + 1) * 32, (localY + 1) * 32, 32, 32}))
			{
				yInc = 0;
				player.pos.y = localY * 32;
			}
		}
		else if (vel.y < 0)
		{
			Rectangle rec = {player.pos.x, player.pos.y + yInc, 32, 32};
			// check top left
			if (!localNodes[(localY - 1) * 256 + localX].passable && CheckCollisionRecs(rec, (Rectangle){localX * 32, (localY - 1) * 32, 32, 32}))
			{
				yInc = 0;
				player.pos.y = localY * 32;
			}

			rec.y = player.pos.y + yInc;
			// check top right
			if (!localNodes[(localY - 1) * 256 + (localX + 1)].passable && CheckCollisionRecs(rec, (Rectangle){(localX + 1) * 32, (localY - 1) * 32, 32, 32}))
			{
				yInc = 0;
				player.pos.y = localY * 32;
			}
		}

		player.pos.x += xInc;
		player.pos.y += yInc;
	}
	else if (mode == MODE_WORLD)
	{
		if (IsKeyPressed(KEY_RIGHT))
		{
			player.world.x++;
			FillLocalMap(&localNodes);
		}
		else if (IsKeyPressed(KEY_LEFT))
		{
			player.world.x--;
			FillLocalMap(&localNodes);
		}
	}
}

void UpdateLocalCamera()
{
	drawTopNeighbor = false;
	drawBottomNeighbor = false;
	drawLeftNeighbor = false;
	drawRightNeighbor = false;

	camera.target.x = (int)(player.pos.x + 16);
	camera.target.y = (int)(player.pos.y + 16);

	if (camera.target.x < 128)
	{
		drawLeftNeighbor = true;
	}

	if (camera.target.y < 128)
	{
		drawTopNeighbor = true;
	}

	if (camera.target.x > 32 * 256 - 128)
	{
		drawRightNeighbor = true;
	}

	if (camera.target.y > 32 * 256 - 128)
	{
		drawBottomNeighbor = true;
	}

	if (camera.target.x < 128 - 16)
	{
		camera.target.x = 128 - 16;
	}

	if (camera.target.y < 128 - 16)
	{
		camera.target.y = 128 - 16;
	}

	if (camera.target.x > 32 * 256 - 128 + 16)
	{
		camera.target.x = 32 * 256 - 128 + 16;
	}

	if (camera.target.y > 32 * 256 - 128 + 16)
	{
		camera.target.y = 32 * 256 - 128 + 16;
	}
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
	UpdateVillage();

	UpdateWorldLoc();

	UpdateGameInput();

	UpdateLocalCamera();
}

void DrawWorld()
{
	DrawTexture(worldTexture, 0, 0, WHITE);
	DrawTexture(villageMarkers, 0, 0, WHITE);
	DrawRectangleLines(player.world.x - 1, player.world.y - 1, 3, 3, RED);
}

void DrawLocal()
{
	int biome = GetBiomeAtWorldLocation(player.world);

	int drawCount = 0;

	if (drawLeftNeighbor && player.world.x > 0)
	{
		Color c = GetBiomeColor(GetBiomeAtWorldLocation((Location){player.world.x - 1, player.world.y}));
		DrawRectangle(0, 0, 32, GetScreenHeight(), c);
	}

	if (drawRightNeighbor && player.world.x < 256)
	{
		Color c = GetBiomeColor(GetBiomeAtWorldLocation((Location){player.world.x + 1, player.world.y}));
		DrawRectangle(GetScreenWidth() - 32, 0, 32, GetScreenHeight(), c);
	}

	if (drawTopNeighbor && player.world.y > 0)
	{
		Color c = GetBiomeColor(GetBiomeAtWorldLocation((Location){player.world.x, player.world.y - 1}));
		DrawRectangle(0, 0, GetScreenWidth(), 32, c);
	}

	if (drawBottomNeighbor && player.world.y < 256)
	{
		Color c = GetBiomeColor(GetBiomeAtWorldLocation((Location){player.world.x, player.world.y + 1}));
		DrawRectangle(0, GetScreenHeight() - 32, GetScreenWidth(), 32, c);
	}

	// draw the nodes of this chunk using camera for perspective.
	BeginMode2D(camera);
	for (int y = (camera.target.y - 128) / 32; y < 256 && y < (camera.target.y - 128) / 32 + 8; y++)
	{
		for (int x = (camera.target.x - 128) / 32; x < 256 && x < (camera.target.x - 128) / 32 + 8; x++)
		{
			DrawTexturePro(tileMap, (Rectangle){0, ((biome * 8) + localNodes[y * 256 + x].tile) * 32, 32, 32}, (Rectangle){x * 32, y * 32, 32, 32}, (Vector2){0, 0}, 0.0f, WHITE);
			int item = localNodes[y * 256 + x].item;
			if (item > -1)
			{
				DrawTexturePro(biomeTexture, (Rectangle){0, item * 32, 32, 32}, (Rectangle){x * 32, y * 32, 32, 32}, (Vector2){0, 0}, 0.0f, WHITE);
				DrawText(TextFormat("%i", item), x * 32, y * 32, GetFontDefault().baseSize, BLACK);
			}
			drawCount++;
		}
	}
	DrawVillage();

	DrawRectangleLines(player.pos.x, player.pos.y, 32, 32, PURPLE);
	EndMode2D();

	DrawStyleTextAnchored(TextFormat("biome %i draw count %i", biome, drawCount), (FontAnchors){-1, -1, -1, 10});
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
	// TODO: Draw GAMEPLAY screen here!
	ClearBackground(BLACK);

	SetCurrentFontStyle("gameplaysmall");

	switch (mode)
	{
	case MODE_WORLD:
		DrawWorld();
		break;
	case MODE_LOCAL:;
		DrawLocal();
		break;
	}

	DrawStyleTextAnchored(TextFormat("wrld: %i/%i", player.world.x, player.world.y), (FontAnchors){10, -1, -1, 10});
	DrawStyleTextAnchored(TextFormat("pos:  %f/%f", player.pos.x, player.pos.y), (FontAnchors){-1, -1, 10, 10});
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
	// TODO: Unload GAMEPLAY screen variables here!
	UnloadFontStyle("gameplaysmall");

	UnloadImage(worldImg);
	UnloadImage(heightMap);
	UnloadImage(tileMapImage);

	UnloadTexture(worldTexture);
	UnloadTexture(tileMap);
	UnloadTexture(itemMap);
	UnloadTexture(villageMarkers);

	UnloadVillage();
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
	return finishScreen;
}