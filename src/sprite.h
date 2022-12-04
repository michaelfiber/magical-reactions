#ifndef SPRITE_H
#define SPRITE_H

#include "raylib.h"

#define MAX_SPRITES 1024

typedef struct {
	bool isActive;
	Texture2D texture;
	Vector2 location;
	int currentFrame;
	float currentDuration;
	float frameDuration;
	int frameCount;
	float rotation;
	Color tint;
	bool isBlocking;
} Sprite;

extern Sprite sprites[MAX_SPRITES];

void ResetSprites();

int AddSprite(Texture2D texture, Vector2 location, float duration);

void UpdateSprite(int spriteIndex);
void UpdateSprites();

void DrawSprite(int spriteIndex);
void DrawSprites();

#endif