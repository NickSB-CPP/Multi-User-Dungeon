#include "connector.h"
#include <stdlib.h>

#define TOTAL_MAPS 3

int get_next_map(int currentMap) {
    int newMap;
    do {
        newMap = rand() % TOTAL_MAPS; // Randomly pick a map ID between 0 and TOTAL_MAPS-1.
    } while(newMap == currentMap);
    return newMap;
}
