#include "raylib.h"
#include "sprite.h"

Sprite sprites[MAX_SPRITES] = {0};

int AddSprite(Texture2D texture, Vector2 location, float duration)
{
	for (int i = 0; i < MAX_SPRITES; i++)
	{
		if (!sprites[i].isActive)
		{
			sprites[i].texture = texture;
			sprites[i].frameCount = texture.width / 32;
			sprites[i].frameDuration = duration;
			sprites[i].currentDuration = duration;
			sprites[i].currentFrame = 0;
			sprites[i].isActive = true;
			sprites[i].location.x = location.x;
			sprites[i].location.y = location.y;
			sprites[i].tint = WHITE;
			sprites[i].isBlocking = true;
			return i;
		}
	}
	return -1;
}

void ResetSprites()
{
	for (int i = 0; i < MAX_SPRITES; i++)
	{
		sprites[i].isActive = false;
	}
}

void UpdateSprite(int spriteIndex)
{
	sprites[spriteIndex].currentDuration -= GetFrameTime();
	if (sprites[spriteIndex].currentDuration < 0)
	{
		sprites[spriteIndex].currentDuration = sprites[spriteIndex].frameDuration;
		sprites[spriteIndex].currentFrame++;
		if (sprites[spriteIndex].currentFrame >= sprites[spriteIndex].frameCount)
			sprites[spriteIndex].currentFrame = 0;
	}
}

void UpdateSprites()
{
	for (int i = 0; i < MAX_SPRITES; i++)
	{
		if (sprites[i].isActive)
			UpdateSprite(i);
	}
}

void DrawSprite(int spriteIndex)
{
	DrawTexturePro(sprites[spriteIndex].texture,
				   (Rectangle){sprites[spriteIndex].currentFrame * 32, 0, 32, 32},
				   (Rectangle){sprites[spriteIndex].location.x, sprites[spriteIndex].location.y, 32, 32},
				   (Vector2){0, 0},
				   sprites[spriteIndex].rotation,
				   sprites[spriteIndex].tint);
}

void DrawSprites()
{
	for (int i = 0; i < MAX_SPRITES; i++)
	{
		if (sprites[i].isActive)
			DrawSprite(i);
	}
}