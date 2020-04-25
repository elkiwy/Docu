#pragma once
#include <stdlib.h>
#include <string.h>

#define MAP_SIZE 32768

typedef struct MapItem{
    char* key;
    void* value;
    struct MapItem* next;
} MapItem;

typedef struct _Map{
    MapItem** items;
} Map;


//djb2 algorithm by dan bernstein
unsigned int hash(char* key);

//Create a new map
Map* map_new();

//Create a new item
MapItem* map_item_new(char* key, void* value);

//Insert into the map
void map_insert(Map* m, char* key, void* value);

//Retrieve from the map
void* map_get(Map* m, char* key);

//Free a map
void map_free(Map* m);

//Free a map item
void map_item_free(MapItem* mi);

