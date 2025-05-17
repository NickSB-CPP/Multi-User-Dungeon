/*
                        +--------------------------+
                    |         Room 2         |
                    | (Connector for modules)|
                    +-----------↑--------------+
                                | (north of Room 0)
                +---------+   +---------+
                | Room 0  |→  | Room 1  |
                | (START)|   +---------+
                +---------+
                    |
                    V
                +---------+
                | Room 3  |
                +---------+
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

extern int currentRoom3;

Room map3_rooms[] = {
    { "Room 0 (START)", "A small clearing at the edge of the woods.", 2, 1, -1, -1 },
    { "Room 1",         "A path leading deeper into the forest.",      -1, -1, 3, 0 },
    { "Room 2 (Connector)", "A portal with ancient carvings.",          -1, -1, 0, -1 },
    { "Room 3",         "The trail winds between dense pines.",        1, -1, 4, -1 },
    { "TREASURE",       "A glimmering reward awaits.",                 3, -1, -1, -1 }
};

void init_map3() {
    currentRoom3 = 0;
}

const char* get_room_description3(int index) {
    int count = sizeof(map3_rooms) / sizeof(Room);
    return (index >= 0 && index < count) ? map3_rooms[index].description : "Unknown room";
}

int move_room3(int current, char direction) {
    int next = -1;
    switch (direction) {
        case 'n': next = map3_rooms[current].north; break;
        case 'e': next = map3_rooms[current].east; break;
        case 's': next = map3_rooms[current].south; break;
        case 'w': next = map3_rooms[current].west; break;
        default: return current;
    }
    return (next == -1 ? current : next);
}

const char* get_fail_message3() {
    const char *messages[] = {
        "An eerie mist holds you back!",
        "The ancient bark of a giant tree deters you!",
        "A sudden chill stops your step!",
        "Mystical runes glow, blocking your way!",
        "An unseen force prevents passage!"
    };
    int count = sizeof(messages) / sizeof(messages[0]);
    return messages[rand() % count];
}
