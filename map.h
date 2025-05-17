#ifndef MAP_H
#define MAP_H

// Map identifiers for future use.
#define MAP1 0
#define MAP2 1
#define MAP3 2

// Define the Room structure with directional connections.
// Use -1 to indicate that there is no room in that direction.
typedef struct {
    const char* name;
    const char* description;
    int north; // Index of the room to the north.
    int east;  // Index of the room to the east.
    int south; // Index of the room to the south.
    int west;  // Index of the room to the west.
} Room;

// Declaration of the map1 array and room count.
extern Room map1_rooms[];
extern int map1_room_count;

// Global variable to track the current room.
extern int currentRoom;

// Function prototypes.
void init_map1();
const char* get_room_description(int index);
int move_room(int current, char direction);

// For Map1 only – returns a randomized failure message when the move is blocked.
const char* get_fail_message();

#endif
