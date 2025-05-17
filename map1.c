/*
                         +--------------------------+
                     |        Room 9          |
                     | (Connector for modules)|
                     +-------------↑----------+
                                   | (south)
                                   |
                +---------+      +---------+      +---------+
                | Room 0  |      | Room 1  |      | Room 2  |
                | (START)|      |         |←────→ |         |
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

*/


#include "map.h"
#include <stdio.h>
#include <stdlib.h>

// Reference global variable defined in dungeon.c.
extern int currentRoom1;

Room map1_rooms[] = {
    { "Room 0 (START)",    "You are at the forest entrance.",           -1, -1, 4, -1 },
    { "Room 1",            "A quiet clearing among ancient trees.",      9, 2, 3, -1 },
    { "Room 2",            "A sunlit area with tall, whispering trees.",   -1, -1, 5, 1 },
    { "Room 3",            "A gloomy path with looming shadows.",          1, -1, 6, 4 },
    { "Room 4",            "A narrow trail bordered by ancient oaks.",     0, 3, 8, -1 },
    { "Room 5",            "A clearing with scattered light.",             2, -1, 7, -1 },
    { "Room 6",            "The path deepens into the forest.",            3, -1, -1, 8 },
    { "Room 7",            "A pathway leading to something unknown.",      5, -1, 10, -1 },
    { "Room 8",            "A hidden alcove, calm but eerie.",             4, 6, -1, -1 },
    { "Room 9 (Connector)","A mystical portal leading elsewhere.",        -1, -1, 1, -1 },
    { "TREASURE",          "Glittering riches lie ahead.",                7, -1, -1, -1 }
};

void init_map1() {
    currentRoom1 = 0;
}

const char* get_room_description1(int index) {
    int count = sizeof(map1_rooms) / sizeof(Room);
    return (index >= 0 && index < count) ? map1_rooms[index].description : "Unknown room";
}

int move_room1(int current, char direction) {
    int next = -1;
    switch (direction) {
        case 'n': next = map1_rooms[current].north; break;
        case 'e': next = map1_rooms[current].east; break;
        case 's': next = map1_rooms[current].south; break;
        case 'w': next = map1_rooms[current].west; break;
        default: return current;
    }
    return (next == -1 ? current : next);
}

const char* get_fail_message1() {
    const char *messages[] = {
        "A huge statue blocks your way!",
        "A massive, gnarled tree prevents passage!",
        "Thick brambles bar your path!",
        "A mysterious fog stops you!",
        "Ancient runes on a stone wall impede you!"
    };
    int count = sizeof(messages) / sizeof(messages[0]);
    return messages[rand() % count];
}