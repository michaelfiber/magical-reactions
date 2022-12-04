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

	srand(worldSeed);
	heightMap = LoadImage("resources/world-height.png");
	worldImg = GenerateWorldImage(heightMap);
	worldTexture = LoadTextureFromImage(worldImg);

	tileMapImage = GenerateTileMap();
	tileMap = LoadTextureFromImage(tileMapImage);

	itemMap = GenerateItemMap();

	player.world.x = 128;
	player.world.y = 128;

	player.pos.x = 3104;
	player.pos.y = 5408;

	player.speed = 200.0f;

	mode = MODE_LOCAL;

	int *data = heightMap.data;

	// this also sets up the village.
	FillLocalMap(&localNodes);

	camera.zoom = 1.0f;
	camera.offset.x = 128;
	camera.offset.y = 128;
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

		player.pos.x += vel.x * GetFrameTime() * player.speed;
		player.pos.y += vel.y * GetFrameTime() * player.speed;
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
	DrawRectangleLines(player.world.x - 1, player.world.y - 1, 3, 3, RED);
	DrawTextureRec(itemMap, (Rectangle){0, 0, 32, itemMap.height}, (Vector2){0, 0}, WHITE);
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
			if (localNodes[y * 256 + x].item > 0)
			{
				DrawTexturePro(biomeTexture, (Rectangle){0, localNodes[y + 256 + x].item * 32, 32, 32}, (Rectangle){x * 32, y * 32, 32, 32}, (Vector2){0, 0}, 0.0f, WHITE);
			}
			drawCount++;

			if (CheckCollisionPointRec((Vector2){player.pos.x + 16, player.pos.y + 16}, (Rectangle){x * 32, y * 32, 32, 32}))
			{
				DrawRectangleLines(x * 32, y * 32, 32, 32, WHITE);
			}
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

	UnloadVillage();
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
	return finishScreen;
}