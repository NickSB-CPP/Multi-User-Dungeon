#include <string.h>
#include "dungeon.h"

void setupRoomsC(Room dungeon[]) {
    dungeon[20].id = 20;
    dungeon[20].type = ROOM_TYPE_START;
    strcpy(dungeon[20].description, "At the edge of an ancient forest, a cobblestone path invites you into an enchanted realm. Rustling leaves and distant echoes fill the air with wonder and eerie anticipation.");
    dungeon[20].east  = 21; dungeon[20].south = 22; dungeon[20].west  = -1; dungeon[20].north = -1;

    dungeon[21].id = 21;
    dungeon[21].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[21].description, "Sunlight streams through towering trees into a clearing where wildflowers dance and soft whispers hint at ancient secrets.");
    dungeon[21].west  = 20; dungeon[21].east  = 23; dungeon[21].north = 24; dungeon[21].south = -1;

    dungeon[22].id = 22;
    dungeon[22].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[22].description, "A gentle brook murmurs over smooth stones, its watery song echoing the forest's forgotten tales.");
    dungeon[22].north = 20; dungeon[22].east  = 25; dungeon[22].west  = -1; dungeon[22].south = -1;
    
    dungeon[23].id = 23;
    dungeon[23].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[23].description, "A ring of massive stone monoliths, etched with cryptic runes, stands in silent vigil—a testament to lost rituals.");
    dungeon[23].west  = 21; dungeon[23].east  = 26; dungeon[23].north = -1; dungeon[23].south = -1;

    dungeon[24].id = 24;
    dungeon[24].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[24].description, "In a secluded grove, ephemeral lights flicker among ancient trees, weaving enchanting spells that whisper of forgotten magic.");
    dungeon[24].south = 21; dungeon[24].east  = 27; dungeon[24].west  = -1; dungeon[24].north = -1;

    dungeon[25].id = 25;
    dungeon[25].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[25].description, "A calm pond mirrors the sky and stirs echoes of a lost past with gentle ripples that disturb its flawless surface.");
    dungeon[25].west  = 22; dungeon[25].south = 27; dungeon[25].east  = -1; dungeon[25].north = -1;

    dungeon[26].id = 26;
    dungeon[26].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[26].description, "Tangled vines weave a natural maze, challenging each step as the forest conceals its secret passageways.");
    dungeon[26].west  = 23; dungeon[26].east  = 28; dungeon[26].north = -1; dungeon[26].south = -1;

    dungeon[27].id = 27;
    dungeon[27].type = ROOM_TYPE_TREASURE;
    strcpy(dungeon[27].description, "Deep within the forest, a crystalline cavern glows with ancient power—a hidden trove of destiny waiting for the worthy.");
    dungeon[27].west  = 24; dungeon[27].north = 25; dungeon[27].east  = -1; dungeon[27].south = -1;

    dungeon[28].id = 28;
    dungeon[28].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[28].description, "A tangle of thorny vines and overgrown foliage forms a natural barrier, where each step teeters on the brink of peril and promise.");
    dungeon[28].west  = 26; dungeon[28].south = 29; dungeon[28].east  = -1; dungeon[28].north = -1;

    dungeon[29].id = 29;
    dungeon[29].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[29].description, "High above the forest, a natural balcony opens to a mesmerizing panorama of starlit heavens—a brief respite for the weary wanderer.");
    dungeon[29].north = 28; dungeon[29].east  = -1; dungeon[29].west  = -1; dungeon[29].south = -1;
}
