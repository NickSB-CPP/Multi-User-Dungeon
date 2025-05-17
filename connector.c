
#include "connector.h"
#include <stdlib.h>

#define TOTAL_MAPS 3

int get_next_map(int currentMap) {
    return (currentMap + 1) % TOTAL_MAPS;
}
