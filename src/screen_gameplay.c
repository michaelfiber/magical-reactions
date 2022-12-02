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
#include "raylib.h"
#include "screens.h"
#include "fontstyle.h"
#include "worldgen.h"

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

typedef struct
{
	Location world;
	Vector2 pos;
	float speed;
} Player;

static Player player = {0};

Image heightMap = {0};
Image worldImg = {0};
Image tileMapImage = {0};

Texture2D worldTexture = {0};
Texture2D tileMap = {0};

LocalWorldNode localNodes[256 * 256] = {0};

/**
 * @brief Used during InitGameplayScreen to randomize worldwide assets.
 */
static int worldSeed = 12345;

static int mode = MODE_WORLD;

Camera2D camera = {0};

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

	LoadFontStyle("gameplaysmall", small);
	SetCurrentFontStyle("gameplaysmall");

	srand(worldSeed);
	heightMap = LoadImage("resources/world-height.png");
	worldImg = GenerateWorldImage(heightMap);
	worldTexture = LoadTextureFromImage(worldImg);

	tileMapImage = GenerateTileMap();
	tileMap = LoadTextureFromImage(tileMapImage);

	player.world.x = 128;
	player.world.y = 128;

	player.pos.x = 8140;
	player.pos.y = 10;

	player.speed = 100.0f;

	mode = MODE_LOCAL;

	int *data = heightMap.data;

	FillLocalMap(player.world, player.pos, &localNodes);

	camera.zoom = 1.0f;
	camera.offset.x = 128;
	camera.offset.y = 128;
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
	// TODO: Update GAMEPLAY screen variables here!

	// Press enter or tap to change to ENDING screen
	// if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
	// {
	//     finishScreen = 1;
	//     PlaySound(fxCoin);
	// }

	if (IsKeyPressed(KEY_SPACE))
	{
		mode++;
		if (mode > MODE_LOCAL)
			mode = MODE_WORLD;
	}

	if (IsKeyDown(KEY_UP))
	{
		player.pos.y -= GetFrameTime() * player.speed;
	}
	else if (IsKeyDown(KEY_DOWN))
	{
		player.pos.y += GetFrameTime() * player.speed;
	}
	else if (IsKeyDown(KEY_LEFT))
	{
		player.pos.x -= GetFrameTime() * player.speed;
	}
	else if (IsKeyDown(KEY_RIGHT))
	{
		player.pos.x += GetFrameTime() * player.speed;
	}

	camera.target.x = (int)(player.pos.x + 16);
	camera.target.y = (int)(player.pos.y + 16);

	if (camera.target.x < 128)
		camera.target.x = 128;
	if (camera.target.y < 128)
		camera.target.y = 128;

	
}

void DrawWorld()
{
	DrawTexture(worldTexture, 0, 0, WHITE);
	DrawRectangleLines(player.world.x - 1, player.world.y - 1, 3, 3, RED);
}

void DrawLocal()
{
	int biome = GetBiomeAtWorldLocation(player.world);

	int drawCount = 0;

	BeginMode2D(camera);
	for (int y = (camera.target.y - 128) / 32; y < 256 && y < (camera.target.y - 128) / 32 + 8; y++)
	{
		for (int x = (camera.target.x - 128) / 32; x < 256 && x < (camera.target.x - 128) / 32 + 8; x++)
		{
			DrawTexturePro(tileMap, (Rectangle){0, ((biome * 8) + localNodes[y * 256 + x].tile) * 32, 32, 32}, (Rectangle){x * 32, y * 32, 32, 32}, (Vector2){0, 0}, 0.0f, WHITE);
			drawCount++;
		}
	}
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
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
	return finishScreen;
}