#ifndef MAP_H
#define MAP_H

// Map identifiers.
#define MAP1 0
#define MAP2 1
#define MAP3 2

// Room structure.
typedef struct {
    const char* name;
    const char* description;
    int north;
    int east;
    int south;
    int west;
} Room;

// Declare global room trackers (do not define them here).
extern int currentRoom1;
extern int currentRoom2;
extern int currentRoom3;

// Declare global room arrays so they can be referenced in other files.
extern Room map1_rooms[];
extern Room map2_rooms[];
extern Room map3_rooms[];

// Function prototypes for MAP1.
void init_map1();
const char* get_room_description1(int index);
int move_room1(int current, char direction);
const char* get_fail_message1();

// Function prototypes for MAP2.
void init_map2();
const char* get_room_description2(int index);
int move_room2(int current, char direction);
const char* get_fail_message2();

// Function prototypes for MAP3.
void init_map3();
const char* get_room_description3(int index);
int move_room3(int current, char direction);
const char* get_fail_message3();

#endif