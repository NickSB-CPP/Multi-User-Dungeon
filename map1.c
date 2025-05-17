#include "map.h"
#include <stdio.h>
#include <stdlib.h>

/*
  Updated Map Layout Diagram:

                     +--------------------------+
                     |         Room 9         |
                     | (Connector for modules)|
                     +-------------↑----------+
                                   | (south)
                                   |
                +---------+      +---------+      +---------+
                | Room 0  |      | Room 1  |      | Room 2  |
                | (START) |      |         |←────→ |         |
                +---------+      +---------+      +---------+
                     |                |                |
                     V                V                V
                +---------+      +---------+      +---------+
                | Room 4  |←──→ | Room 3  |      | Room 5  |
                +---------+      +---------+      +---------+
                     |                |                |
                     V                V                V
                +---------+      +---------+      +---------+
                | Room 8  |←──→ | Room 6  |      | Room 7  |
                +---------+      +---------+      +---------+
                                                  |
                                                  V
                                             +---------+
                                             |TREASURE |
                                             +---------+
  
  Room Index Mapping:
    0  - Room 0 (START)
    1  - Room 1
    2  - Room 2
    3  - Room 3
    4  - Room 4
    5  - Room 5
    6  - Room 6
    7  - Room 7
    8  - Room 8
    9  - Room 9 (Connector)
    10 - TREASURE
*/

Room map1_rooms[] = {
    // Format: { name, description, north, east, south, west }
    { "Room 0 (START)",    "You are at the forest entrance.",           -1,     -1,   4,  -1 }, // 0  
    { "Room 1",            "A quiet clearing among ancient trees.",      9,      2,   3,  -1 }, // 1  
    { "Room 2",            "A sunlit area with tall, whispering trees.",   -1,    -1,   5,   1 }, // 2  
    { "Room 3",            "A gloomy path with looming shadows.",          1,     -1,   6,   4 }, // 3  
    { "Room 4",            "A narrow trail bordered by ancient oaks.",     0,      3,   8,  -1 }, // 4  
    { "Room 5",            "A clearing with scattered light.",             2,     -1,   7,  -1 }, // 5  
    { "Room 6",            "The path deepens into the forest.",            3,     -1,  -1,   8 }, // 6  
    { "Room 7",            "A pathway that leads mysteriously onward.",    5,     -1,  10,  -1 }, // 7  
    { "Room 8",            "A hidden alcove with surprising calm.",        4,      6,  -1,  -1 }, // 8  
    { "Room 9 (Connector)","A magical doorway connecting realms.",        -1,    -1,   1,  -1 }, // 9  
    { "TREASURE",          "Glittering treasures lie hidden here.",         7,     -1,  -1,  -1 }  // 10
};

int map1_room_count = sizeof(map1_rooms) / sizeof(Room);
int currentRoom = 0;

void init_map1() {
    currentRoom = 0; // Start at Room 0.
    printf("Map1 initialized. Total rooms: %d\n", map1_room_count);
}

const char* get_room_description(int index) {
    if (index < 0 || index >= map1_room_count) {
        return "Unknown room";
    }
    return map1_rooms[index].description;
}

// move_room() returns the new room index if you can move; otherwise, it returns current.
int move_room(int current, char direction) {
    int next = -1;
    switch (direction) {
        case 'n': next = map1_rooms[current].north; break;
        case 'e': next = map1_rooms[current].east;  break;
        case 's': next = map1_rooms[current].south; break;
        case 'w': next = map1_rooms[current].west;  break;
        default:
            printf("Invalid direction.\n");
            return current;
    }
    if (next == -1) {
        // Do not print here; let the server handle the failure message.
        return current;
    }
    return next;
}

// get_fail_message() returns a random failure message.
const char* get_fail_message() {
    const char *messages[] = {
        "A huge statue blocks your way!",
        "A massive, gnarled tree prevents passage!",
        "Thick brambles and vines bar your path!",
        "A mysterious fog stops you in your tracks!",
        "Ancient runes on a crumbling stone wall impede you!"
    };
    int count = sizeof(messages) / sizeof(messages[0]);
    int idx = rand() % count;
    return messages[idx];
}
