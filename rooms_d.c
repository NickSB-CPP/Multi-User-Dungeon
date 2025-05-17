#include <string.h>
#include "dungeon.h"

void setupRoomsD(Room dungeon[]) {
    dungeon[30].id = 30;
    dungeon[30].type = ROOM_TYPE_START;
    strcpy(dungeon[30].description, "You awaken in a high-security control center, alarms long dead.");
    dungeon[30].south = 31; dungeon[30].north = -1; dungeon[30].east = -1; dungeon[30].west = -1;

    dungeon[31].id = 31;
    dungeon[31].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[31].description, "A wall of broken monitors flickers with static. You feel watched.");
    dungeon[31].north = 30; dungeon[31].south = 32; dungeon[31].east = -1; dungeon[31].west = -1;

    dungeon[32].id = 32;
    dungeon[32].type = ROOM_TYPE_CONNECTOR;
    strcpy(dungeon[32].description, "A decontamination hallway lined with long-dead security systems.");
    dungeon[32].north = 31; dungeon[32].south = 33; dungeon[32].east = -1; dungeon[32].west = -1;

    dungeon[33].id = 33;
    dungeon[33].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[33].description, "Server racks hum with residual energy. Something beeps faintly.");
    dungeon[33].north = 32; dungeon[33].south = 34; dungeon[33].east = -1; dungeon[33].west = -1;

    dungeon[34].id = 34;
    dungeon[34].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[34].description, "The hallway is scorched. Evidence of a past explosion lingers.");
    dungeon[34].north = 33; dungeon[34].south = 35; dungeon[34].east = -1; dungeon[34].west = -1;

    dungeon[35].id = 35;
    dungeon[35].type = ROOM_TYPE_CONNECTOR;
    strcpy(dungeon[35].description, "A drone bay, now abandoned. Mechanical arms twitch occasionally. A strange shimmering panel stands to the east.");
    dungeon[35].north = 34; dungeon[35].south = 36; dungeon[35].east = -2; dungeon[35].west = -1;

    dungeon[36].id = 36;
    dungeon[36].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[36].description, "Maintenance shaft. Flickering lights. Access panels hang open.");
    dungeon[36].north = 35; dungeon[36].south = 37; dungeon[36].east = -1; dungeon[36].west = -1;

    dungeon[37].id = 37;
    dungeon[37].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[37].description, "Observation room with shattered glass. A red light blinks overhead.");
    dungeon[37].north = 36; dungeon[37].south = 38; dungeon[37].east = -1; dungeon[37].west = -1;

    dungeon[38].id = 38;
    dungeon[38].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[38].description, "Backup generator room. The hum is louder here, almost alive.");
    dungeon[38].north = 37; dungeon[38].south = 39; dungeon[38].east = -1; dungeon[38].west = -1;

    dungeon[39].id = 39;
    dungeon[39].type = ROOM_TYPE_TREASURE;
    strcpy(dungeon[39].description, "Central command vault. Glowing AI core rests on a pedestal.");
    dungeon[39].north = 38; dungeon[39].south = -1; dungeon[39].east = -1; dungeon[39].west = -1;
}
