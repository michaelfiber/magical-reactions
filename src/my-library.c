#include "my-library.h"

#ifndef PLATFORM_WEB

void Say(char *text) {}
void RegisterOrientationPointers(float *beta, float *gamma) {}
void Vibrate(int *durations, int durationCount) {}

#endif