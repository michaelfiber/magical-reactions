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
} Player;

static Player player = {0};

Image heightMap = {0};
Image worldImg = {0};
Image tileMapImage = {0};

Texture2D worldTexture = {0};
Texture2D tileMap = {0};

/**
 * @brief Used during InitGameplayScreen to randomize worldwide assets.
 */
static int worldSeed = 12345;

static int mode = MODE_WORLD;

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

	player.pos.x = 128;
	player.pos.y = 128;

	mode = MODE_LOCAL;
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
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
	// TODO: Draw GAMEPLAY screen here!
	ClearBackground(BLACK);

	switch (mode)
	{
	case MODE_WORLD:
		DrawTexture(worldTexture, 0, 0, WHITE);
		DrawRectangleLines(player.world.x - 1, player.world.y - 1, 3, 3, RED);
		break;
	case MODE_LOCAL:
		DrawTexture(tileMap, 0, 0, WHITE);
		DrawRectangleLines(0, 0, tileMap.width, tileMap.height, RED);
		break;
	}

	SetCurrentFontStyle("gameplaysmall");
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