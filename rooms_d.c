#include <string.h>
#include "dungeon.h"

void setupRoomsD(Room dungeon[]) {
    dungeon[30].id = 30;
    dungeon[30].type = ROOM_TYPE_START;
    strcpy(dungeon[30].description, "You awaken in a sealed docking bay. Water drips from the ceiling. Lights flicker dimly.");
    dungeon[30].east = 31; dungeon[30].north = -1; dungeon[30].south = -1; dungeon[30].west = -1;

    dungeon[31].id = 31;
    dungeon[31].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[31].description, "An observation corridor lined with cracked glass reveals the dark ocean beyond.");
    dungeon[31].west = 30; dungeon[31].east = 32; dungeon[31].south = 34; dungeon[31].north = -1;

    dungeon[32].id = 32;
    dungeon[32].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[32].description, "Storage pods lie shattered. A strange growth creeps across the walls.");
    dungeon[32].west = 31; dungeon[32].east = 33; dungeon[32].north = -1; dungeon[32].south = -1;

    dungeon[33].id = 33;
    dungeon[33].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[33].description, "The lab is trashed. Data logs blink red. A shattered helmet lies on the floor.");
    dungeon[33].west = 32; dungeon[33].east = -1; dungeon[33].north = -1; dungeon[33].south = -1;

    dungeon[34].id = 34;
    dungeon[34].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[34].description, "A utility junction. Rusted pipes groan. Water seeps through a crack.");
    dungeon[34].north = 31; dungeon[34].south = 35; dungeon[34].east = 36; dungeon[34].west = -1;

    dungeon[35].id = 35;
    dungeon[35].type = ROOM_TYPE_CONNECTOR;
    strcpy(dungeon[35].description, "Narrow maintenance shaft. Crawling is the only option forward.");
    dungeon[35].north = 34; dungeon[35].south = 37; dungeon[35].east = -1; dungeon[35].west = -1;

    dungeon[36].id = 36;
    dungeon[36].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[36].description, "An eerie silence fills the hydroponics lab. Broken tanks spill nutrient fluid.");
    dungeon[36].west = 34; dungeon[36].south = 38; dungeon[36].north = -1; dungeon[36].east = -1;

    dungeon[37].id = 37;
    dungeon[37].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[37].description, "Emergency quarantine zone. The door behind you locks shut.");
    dungeon[37].north = 35; dungeon[37].east = 38; dungeon[37].south = -1; dungeon[37].west = -1;

    dungeon[38].id = 38;
    dungeon[38].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[38].description, "Power core access. Faint humming vibrates the floor. It's still active.");
    dungeon[38].north = 36; dungeon[38].south = 39; dungeon[38].west = 37; dungeon[38].east = -1;

    dungeon[39].id = 39;
    dungeon[39].type = ROOM_TYPE_TREASURE;
    strcpy(dungeon[39].description, "In a sealed vault, you find the station's AI core. It pulses with latent energy.");
    dungeon[39].north = 38; dungeon[39].east = -1; dungeon[39].south = -1; dungeon[39].west = -1;
}