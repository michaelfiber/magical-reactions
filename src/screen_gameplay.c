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

#include "raylib.h"
#include "screens.h"
#include "fontstyle.h"
#include "worldgen.h"

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
    Vector2 pos;
    WorldThing thing;
} Player;

static Player player = {0};

Texture2D tiles = {0};

static int worldSeed = 12345;

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

	getWorld(worldSeed);
	Image img = genTiles(worldSeed);
	tiles = LoadTextureFromImage(img);
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
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!
    ClearBackground(BLUE);

	DrawTexture(tiles, 0, 0, WHITE);

    SetCurrentFontStyle("gameplaysmall");
    DrawStyleTextAnchored(TextFormat("wrld: %i/%i", player.thing.world.x, player.thing.world.y), (FontAnchors){10, -1, -1, 10});
    DrawStyleTextAnchored(TextFormat("regn: %i/%i", player.thing.region.x, player.thing.region.y), (FontAnchors){-1, -1, -1, 10});
    DrawStyleTextAnchored(TextFormat("locl: %i/%i", player.thing.local.x, player.thing.local.y), (FontAnchors){-1, -1, 10, 10});
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
    UnloadFontStyle("gameplaysmall");
	UnloadTexture(tiles);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}