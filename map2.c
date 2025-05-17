/*
                   +----------------------+
                   |       Room 6         |
                   | (Connector for maps) |
                   +-----------↑----------+
                               | (north of Room0 & Room1)
                +---------+   +---------+
                | Room 0  |   | Room 1  |
                | (START)|   |         |
                +---------+   +---------+
                     |              |
                     V              V
                +---------+   +---------+
                | Room 2  |   | Room 3  |
                +---------+   +---------+
                     |              |
                     V              V
                +---------+   +---------+
                | Room 4  |   | Room 5  |
                +---------+   +---------+
                               |
                               V
                          +---------+
                          |TREASURE |
                          +---------+

    (demo)
*/

#include "map.h"
#include <stdio.h>
#include <stdlib.h>

extern int currentRoom2;

Room map2_rooms[] = {
    { "Room 0 (START)", "You stand at a mossy trail.",         6, 1, 2, -1 },
    { "Room 1",         "A clearing with dappled light.",         6, -1, 3, 0 },
    { "Room 2",         "A path winding through ferns.",          0, 3, 4, -1 },
    { "Room 3",         "The forest thickens.",                   1, -1, 5, 2 },
    { "Room 4",         "A narrow trail along ancient trees.",      2, 5, -1, -1 },
    { "Room 5",         "The air grows chilly here.",               3, -1, 7, -1 },
    { "Room 6 (Connector)", "A mystical archway shrouded in mist.",   -1, -1, 0, -1 },
    { "TREASURE",       "A chamber of golden light.",              5, -1, -1, -1 }
};

void init_map2() {
    currentRoom2 = 0;
}

const char* get_room_description2(int index) {
    int count = sizeof(map2_rooms) / sizeof(Room);
    return (index >= 0 && index < count) ? map2_rooms[index].description : "Unknown room";
}

int move_room2(int current, char direction) {
    int next = -1;
    switch (direction) {
        case 'n': next = map2_rooms[current].north; break;
        case 'e': next = map2_rooms[current].east; break;
        case 's': next = map2_rooms[current].south; break;
        case 'w': next = map2_rooms[current].west; break;
        default: return current;
    }
    return (next == -1 ? current : next);
}

const char* get_fail_message2() {
    const char *messages[] = {
        "A fallen log blocks your way!",
        "Dense briars prevent progress!",
        "An ancient stump stops you!",
        "A thick veil of vines bars passage!",
        "The undergrowth is too wild to proceed!"
    };
    int count = sizeof(messages) / sizeof(messages[0]);
    return messages[rand() % count];
}