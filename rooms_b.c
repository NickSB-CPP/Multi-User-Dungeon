#include <string.h>
#include "dungeon.h"

void setupRoomsB(Room dungeon[]) {
    dungeon[10].id = 10;
    dungeon[10].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[10].description, "You step into a new wing — eerily silent and recently abandoned.");
    dungeon[10].east = 11;
    dungeon[10].west = 1;
    dungeon[10].north = -1;
    dungeon[10].south = -1;

    dungeon[11].id = 11;
    dungeon[11].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[11].description, "The hallway narrows, the walls lined with rusted pipes.");
    dungeon[11].west = 10;
    dungeon[11].east = 12;
    dungeon[11].north = -1;
    dungeon[11].south = -1;

    dungeon[12].id = 12;
    dungeon[12].type = ROOM_TYPE_CONNECTOR;
    strcpy(dungeon[12].description, "A connector room with debris scattered across the floor.");
    dungeon[12].west = 11;
    dungeon[12].east = 13;
    dungeon[12].north = -1;
    dungeon[12].south = -1;

    dungeon[13].id = 13;
    dungeon[13].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[13].description, "Storage room. Empty crates and broken lockers everywhere.");
    dungeon[13].west = 12;
    dungeon[13].east = 14;
    dungeon[13].north = -1;
    dungeon[13].south = -1;

    dungeon[14].id = 14;
    dungeon[14].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[14].description, "Dimly lit archives. Loose papers and torn binders litter the floor.");
    dungeon[14].west = 13;
    dungeon[14].east = 15;
    dungeon[14].north = -1;
    dungeon[14].south = -1;

    dungeon[15].id = 15;
    dungeon[15].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[15].description, "A room full of twisted machinery that hums faintly.");
    dungeon[15].west = 14;
    dungeon[15].east = 16;
    dungeon[15].north = -1;
    dungeon[15].south = -1;

    dungeon[16].id = 16;
    dungeon[16].type = ROOM_TYPE_CONNECTOR;
    strcpy(dungeon[16].description, "Transition room — footprints in the dust suggest someone passed recently.");
    dungeon[16].west = 15;
    dungeon[16].east = 17;
    dungeon[16].north = -1;
    dungeon[16].south = -1;

    dungeon[17].id = 17;
    dungeon[17].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[17].description, "You step into a laboratory. Vials and wires cover every surface.");
    dungeon[17].west = 16;
    dungeon[17].east = 18;
    dungeon[17].north = -1;
    dungeon[17].south = -1;

    dungeon[18].id = 18;
    dungeon[18].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[18].description, "Control room. Terminals flicker with static.");
    dungeon[18].west = 17;
    dungeon[18].east = 19;
    dungeon[18].north = -1;
    dungeon[18].south = -1;

    dungeon[19].id = 19;
    dungeon[19].type = ROOM_TYPE_TREASURE;
    strcpy(dungeon[19].description, "Final vault. A locked chest glows with a mysterious light.");
    dungeon[19].west = 18;
    dungeon[19].east = -1;
    dungeon[19].north = -1;
    dungeon[19].south = -1;
}
