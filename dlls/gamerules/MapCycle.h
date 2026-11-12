#ifndef MAPCYCLE_H
#define MAPCYCLE_H

#include <time.h>

#define MAX_MAPCYCLE_NAME 32

struct mapcycle_item_s
{
	mapcycle_item_s    *next;
	char                mapname[MAX_MAPCYCLE_NAME + 1];
	int                 minplayers;
	int                 maxplayers;
	char                rulebuffer[1024];
};

typedef mapcycle_item_s mapcycle_item_t;

struct mapcycle_s
{
	char                filename[MAX_PATH + 1];
	time_t              modified;
	mapcycle_item_t    *items;
	mapcycle_item_t    *next_item;
	int                 count;
	unsigned int        position;
	bool                shuffled;
	unsigned int        parted;
};

typedef mapcycle_s mapcycle_t;

extern mapcycle_t g_mapcycle;

void DestroyMapCycle( mapcycle_t *cycle );
bool ReloadMapCycleFile( char *filename, mapcycle_t *cycle, bool shuffle );

#endif
