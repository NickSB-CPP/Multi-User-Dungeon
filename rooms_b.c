
#include <string.h>
#include "dungeon.h"
//Nicholas Barkero's room
void setupRoomsB(Room dungeon[]) {
    dungeon[10].id = 10;
    dungeon[10].type = ROOM_TYPE_START;
    strcpy(dungeon[10].description, "In a cramped, dark room, all you can feel is the stone walls around you. The air thickens, distant clanks of metal and footsteps echo in the distance.");
    dungeon[10].east = -1; dungeon[10].west = -1; dungeon[10].north = -1; dungeon[10].south = 11;

    dungeon[11].id = 11;
    dungeon[11].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[11].description, "You step outside, you can feel the breeze. Despite that, no light is present. Shuffling around, you find you are surrounded by stone walls and you feel an arrow on a target.");
    dungeon[11].west = -1; dungeon[11].east = 13; dungeon[11].north = 10; dungeon[11].south = 12;

    dungeon[12].id = 12;
    dungeon[12].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[12].description, "You step out and feel a chair and the edge of a table. Putting your hand and moving across the table, it is incredibly long, as if it was made for an army. Where is everyone?");
    dungeon[12].west = -1; dungeon[12].east = 15; dungeon[12].north = 14; dungeon[12].south = -1;

    dungeon[13].id = 13;
    dungeon[13].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[13].description, "Into a corridor you go. Moving your hand along the wall, you feel paintings. You retract it to not potentially damage them. You smell smoke to the west.");
    dungeon[13].west = 11; dungeon[13].east = 15; dungeon[13].north = 14; dungeon[13].south = -1;

    dungeon[14].id = 14;
    dungeon[14].type = ROOM_TYPE_TREASURE;
    strcpy(dungeon[14].description, "Ancient relics, weapons, gold... As you approach the room, you finally see light... As the moon shines upon all the metal before you... Your journey is finally over.");
    dungeon[14].west = -1; dungeon[14].east = -1; dungeon[14].north = -1; dungeon[14].south = -1;

    dungeon[15].id = 15;
    dungeon[15].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[15].description, "You hear chains hanging and clanking. You hear... no feel breathing? And heat coming from the breath? Hearing low rumbling sounds and huffs, it may be in your best interest TO NOT GO WEST.");
    dungeon[15].west = 40; dungeon[15].east = -1; dungeon[15].north = 18; dungeon[15].south = 17;

    dungeon[16].id = 16;
    dungeon[16].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[16].description, "Entering from the kitchen, you feel around and find a blanket. Clearly, you have reached a resting point. A voice in your head tells you to lie down. You are so so tried. However, you must turn around and press on.");
    dungeon[16].west = -1; dungeon[16].east = -1; dungeon[16].north = 12; dungeon[16].south = -1;

    dungeon[17].id = 17;
    dungeon[17].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[17].description, "Stepping down slowly, holding onto the railing, you find the rails end and so do the steps. You smell smoke to the north, but almost trip over shackles and chains. Then, you bump into legs... hanging from the ceiling? Best not to stay long...");
    dungeon[17].west = 16; dungeon[17].east = 12; dungeon[17].north = 15; dungeon[17].south = -1;

    dungeon[18].id = 18;
    dungeon[18].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[18].description, "Smelling smoke behind you, you climb stairs around the stone wall. From there, you feel the outside breeze once more. You can intuitively tell you are on the roof. Do you go back or push forward?");
    dungeon[18].west = 19; dungeon[18].east = -1; dungeon[18].north = -1; dungeon[18].south = 15;

    dungeon[19].id = 19;
    dungeon[19].type = ROOM_TYPE_CONNECTOR;
    strcpy(dungeon[19].description, "A light shines through... eminating from your very being... leaving it behind... you're filled with determination.");
    dungeon[19].west = 18; dungeon[19].east = -2; dungeon[19].north = -1; dungeon[19].south = -1;

    dungeon[40].id = 40;
    dungeon[40].type = ROOM_TYPE_TREASURE;
    strcpy(dungeon[40].description, "YOU DIED");
    dungeon[40].west = -1; dungeon[40].east = -1; dungeon[40].north = -1; dungeon[40].south = -1;
}
