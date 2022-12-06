#include "raylib.h"
#include "myrandom.h"

void DrawImageRec(Color *data, int x, int y, int width, int height, Color color)
{
	for (int xf = x; xf < x + width; xf++)
	{
		for (int yf = y; yf < y + height; yf++)
		{
			data[yf * 16 + xf] = color;
		}
	}
}

Image GetHumanImage()
{
	TraceLog(LOG_INFO, "Generating human...");

	Color transparent = ColorAlpha(WHITE, 0.0f);

	Color *data = (Color *)MemAlloc(sizeof(Color) * 16 * 32);

	Color hairColor = ColorFromHSV(Random(0, 30), Random(0, 9) / 10.0f, Random(0, 9) / 10.0f);
	Color skinColor = ColorFromHSV(175 + Random(0, 39) - 20, Random(6, 9) / 10.0f, Random(6, 9) / 10.0f);
	Color shirtColor = ColorFromHSV(Random(0, 180) - 180, Random(7, 9) / 10.0f, Random(0, 9) / 10.0f);
	Color pantsColor = ColorFromHSV(Random(0, 180) - 180, Random(6, 8) / 10.0f, Random(0, 7) / 10.0f);

	for (int i = 0; i < 16 * 32; i++)
	{
		data[i] = transparent;
	}

	DrawImageRec(data, 4, 20, 8, 12, pantsColor);
	DrawImageRec(data, 2, 8, 12, 12, shirtColor);
	DrawImageRec(data, 4, 0, 8, 6, hairColor);
	DrawImageRec(data, 6, 3, 4, 6, skinColor);

	return (Image){
		.data = data,
		.width = 16,
		.height = 32,
		.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
		.mipmaps = 1};
}