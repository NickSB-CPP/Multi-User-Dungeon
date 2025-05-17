#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dungeonRoom.h"

Room* create_room(const char* name, const char* description, const char* item) {
    Room* r = malloc(sizeof(Room));
    strcpy(r->name, name);
    strcpy(r->description, description);
    strcpy(r->item, item);
    r->north = r->south = r->east = r->west = NULL;
    return r;
}

void describe_room(Room* r) {
    printf("\n== %s ==\n%s\nItem: %s\nExits: ", r->name, r->description, r->item);
    if (r->north) printf("North ");
    if (r->south) printf("South ");
    if (r->east)  printf("East ");
    if (r->west)  printf("West ");
    printf("\n");
}
