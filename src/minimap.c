#include "minimap.h"

//djb2 algorithm by dan bernstein
unsigned int hash(char* key){
    unsigned long value = 5381;
    for(unsigned int i=0; i<strlen(key); ++i){
        value = ((value << 5) + value) + (int)key[i]; /* value * 33 + key[i] */
    }
    return value % MAP_SIZE;
}

//Create a new map
Map* map_new(){
    Map* m = malloc(sizeof(Map));
    m->items = calloc(MAP_SIZE, sizeof(MapItem*));
    for(int i=0; i<MAP_SIZE; ++i){
        m->items[i] = NULL;
    }
    return m;
}

//Create a new item
MapItem* map_item_new(char* key, void* value){
    MapItem* item = malloc(sizeof(MapItem));
    item->key = key;
    item->value = value;
    item->next = NULL;
    return item;
}

//Insert into the map
void map_insert(Map* m, char* key, void* value){
    unsigned int slot = hash(key);
    MapItem* item = m->items[slot];
    //No collision
    if (item==NULL){
        m->items[slot] = map_item_new(key, value);
        return;
    //Collision
    }else{
        MapItem* prevItem = NULL;
        //Cycle through the item in the row...
        while (item!=NULL){
            //...If I found the current Key...
            if (strcmp(item->key, key) == 0){ 
                //...replace the value to the existing item
                item->value = value;
                return;
            }else{
                //Keep looking and save from where I come
                prevItem = item;
                item = item->next;
            }        
        }
        //If I didn't find a matching key create a new one
        prevItem->next = map_item_new(key, value);
        return;
    }
}

//Retrieve from the map
void* map_get(Map* m, char* key){
    unsigned int slot = hash(key);
    MapItem* item = m->items[slot];
    if (item == NULL){
        return NULL;
    }else{
        while(item!=NULL){
            if (strcmp(item->key, key) == 0){
                return item->value;
            }
            item = item->next;
        }
        return NULL;
    }
}
