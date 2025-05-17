#include <string.h>
#include "dungeon.h"

void setupRoomsC(Room dungeon[]) {
    dungeon[20].id = 20;
    dungeon[20].type = ROOM_TYPE_START;
    strcpy(dungeon[20].description, "You enter a jungle-surrounded ruins corridor, the walls overtaken by vines.");
    dungeon[20].east = 21;
    dungeon[20].west = -1;
    dungeon[20].north = -1;
    dungeon[20].south = -1;

    dungeon[21].id = 21;
    dungeon[21].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[21].description, "Stone tablets line the walls. Faded carvings depict forgotten rituals.");
    dungeon[21].east = 22;
    dungeon[21].west = 20;
    dungeon[21].north = -1;
    dungeon[21].south = -1;

    dungeon[22].id = 22;
    dungeon[22].type = ROOM_TYPE_CONNECTOR;
    strcpy(dungeon[22].description, "A collapsed ceiling makes progress difficult. You squeeze through.");
    dungeon[22].east = 23;
    dungeon[22].west = 21;
    dungeon[22].north = -1;
    dungeon[22].south = -1;

    dungeon[23].id = 23;
    dungeon[23].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[23].description, "A giant statue stands in silence. Something about it feels alive.");
    dungeon[23].east = 24;
    dungeon[23].west = 22;
    dungeon[23].north = -1;
    dungeon[23].south = -1;

    dungeon[24].id = 24;
    dungeon[24].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[24].description, "You find a stone kitchen, overgrown and moss-covered.");
    dungeon[24].east = 25;
    dungeon[24].west = 23;
    dungeon[24].north = -1;
    dungeon[24].south = -1;

    dungeon[25].id = 25;
    dungeon[25].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[25].description, "A chamber filled with ancient tools and ritual artifacts.");
    dungeon[25].east = 26;
    dungeon[25].west = 24;
    dungeon[25].north = -1;
    dungeon[25].south = -1;

    dungeon[26].id = 26;
    dungeon[26].type = ROOM_TYPE_CONNECTOR;
    strcpy(dungeon[26].description, "The ground here trembles faintly. A secret may be nearby.");
    dungeon[26].east = 27;
    dungeon[26].west = 25;
    dungeon[26].north = -1;
    dungeon[26].south = -1;

    dungeon[27].id = 27;
    dungeon[27].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[27].description, "You find yourself at the edge of a sacred ritual platform.");
    dungeon[27].east = 28;
    dungeon[27].west = 26;
    dungeon[27].north = -1;
    dungeon[27].south = -1;

    dungeon[28].id = 28;
    dungeon[28].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[28].description, "A cracked obelisk hums faintly. Its runes glow when touched.");
    dungeon[28].east = 29;
    dungeon[28].west = 27;
    dungeon[28].north = -1;
    dungeon[28].south = -1;

    dungeon[29].id = 29;
    dungeon[29].type = ROOM_TYPE_TREASURE;
    strcpy(dungeon[29].description, "You enter the heart of the ruins. A radiant gem floats midair, untouched by time.");
    dungeon[29].west = 28;
    dungeon[29].east = -1;
    dungeon[29].north = -1;
    dungeon[29].south = -1;
}