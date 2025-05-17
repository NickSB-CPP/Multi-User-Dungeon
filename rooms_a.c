#include <string.h>
#include "dungeon.h"

void setupRoomsA(Room dungeon[]) {
    dungeon[0].id = 0;
    dungeon[0].type = ROOM_TYPE_START;
    strcpy(dungeon[0].description, "You awaken on the cold, unforgiving stone floor of a dank prison cell, your head throbbing and your memories clouded. As you start to regain your senses, you smell a stinky smell. You realise that it's the smell of a dead corpse and you notice the corpse in the corner of the room. You see tallies on the walls and you realise your days are numbered.");
    dungeon[0].north = 1; dungeon[0].east = -1; dungeon[0].south = 4; dungeon[0].west = -1;

    dungeon[1].id = 1;
    dungeon[1].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[1].description, "A single chair is bolted to the floor under a hanging light. Dried stains mark the floor, and a broken one-way mirror reveals a small, hidden room behind the wall. Echoes of past screams seem to linger unnaturally.");
    dungeon[1].north = -1; dungeon[1].east = 10; dungeon[1].south = 0; dungeon[1].west = 9;

    dungeon[2].id = 2;
    dungeon[2].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[2].description, "You find yourself in what seems to be a medical ward. Cabinets are open, but most supplies are missing. A patient's journal is stained with ink and blood, ending mid-sentence.");
    dungeon[2].north = -1; dungeon[2].east = 5; dungeon[2].south = 3; dungeon[2].west = 1;

    dungeon[3].id = 3;
    dungeon[3].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[3].description, "You find yourself in a solitary confinement room. The walls are bare white and the air is stale. A single light flickers above, casting eerie shadows.");
    dungeon[3].north = 2; dungeon[3].east = -1; dungeon[3].south = 4; dungeon[3].west = -1;

    dungeon[4].id = 4;
    dungeon[4].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[4].description, "A charred electric chair sits under a broken spotlight. The walls are lined with tally marks and old graffiti. A burnt smell lingers in the air, and the switch still clicks ominously when flipped.");
    dungeon[4].north = 3; dungeon[4].east = -1; dungeon[4].south = 0; dungeon[4].west = -1;

    dungeon[5].id = 5;
    dungeon[5].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[5].description, "Lavish compared to the rest of the prison, this room is dusty but intact. A desk, a locked safe, and a family portrait are here. A false floorboard creaks under pressure, and a hidden door is outlined in the back wall.");
    dungeon[5].north = -1; dungeon[5].east = -1; dungeon[5].south = 6; dungeon[5].west = 2;

    dungeon[6].id = 6;
    dungeon[6].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[6].description, "Mold creeps up the tiled walls. The sound of dripping water echoes endlessly. The pipes groan under pressure, and sometimes spray scalding steam without warning. A rusted drain cover hides something trapped beneath it.");
    dungeon[6].north = 5; dungeon[6].east = -1; dungeon[6].south = 7; dungeon[6].west = -1;

    dungeon[7].id = 7;
    dungeon[7].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[7].description, "Sweltering heat and hissing steam fill this chamber. The machinery is mostly defunct, except for one gauge that still twitches. An old maintenance journal ends with: \"They never left.\"");
    dungeon[7].north = -1; dungeon[7].east = -1; dungeon[7].south = -1; dungeon[7].west = 8;

    dungeon[8].id = 8;
    dungeon[8].type = ROOM_TYPE_TREASURE;
    strcpy(dungeon[8].description, "The chairs and phones are intact, but dust-covered. One phone receiver is off the hook, and if picked up, a voice whispers faintly. The logbook has a name circled repeatedly with the word \"DENIED.\"");
    dungeon[8].north = -1; dungeon[8].east = -1; dungeon[8].south = -1; dungeon[8].west = -1;

    dungeon[9].id = 9;
    dungeon[9].type = ROOM_TYPE_CONNECTOR;
    strcpy(dungeon[9].description, "Visitation Room: Dust-covered chairs and phones. One receiver is off the hook, whispering faintly.");
    dungeon[9].north = -1; dungeon[9].east = 1; dungeon[9].south = -1; dungeon[9].west = -1;
}
