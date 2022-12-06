#ifndef VILLAGE_H
#define VILLAGE_H

#include "worldgen.h"

#define DISPO_ALONE 			0
#define DISPO_LIKES_EVERYONE 	1
#define DISPO_LIKES_SELVES		2
#define DISPO_NEUTRAL			3
#define DISPO_HATES_YOU			4
#define DISPO_HATES_EVERYONE	5

#define DISPO_COUNT 			6

extern bool VillagePassableGrid[256 * 256];
extern int VillageDisposition;

typedef struct {
	int houseType;
	Location loc;
	int gridWidth;
	int gridHeight;
} VillageNode;

bool HasVillageDestructive(Location location);
void InitVillage();
void DrawVillage();
void UpdateVillage();
void UnloadVillage();

#endif