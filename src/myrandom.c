#include "stdlib.h"

void SeedRandom(int seed)
{
	srand(seed);
#ifdef PLATFORM_WEB
	// rand on emscripten gives very little randomness so try to expand the randomness slightly?
	int count = rand() % 3;
	for (int i = 0; i < count; i++)
	{
		srand(rand());
	}
#endif
}

int Random(int min, int max)
{
	return (int)(rand() * 1.0f / RAND_MAX * (max - min)) + min;
}