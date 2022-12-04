#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "worldgen.h"

typedef struct
{
	Location world;
	Vector2 pos;
	float speed;
} Player;

// defined in screen_gameplay.c
extern Player player;

#endif