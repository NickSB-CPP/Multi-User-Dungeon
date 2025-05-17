#ifndef DUNGEONROOM_H
#define DUNGEONROOM_H

typedef struct Room {
    char name[50];             // Internal room name (not shown to player)
    char description[200];     // Little details
    char item[50];             // Could be "Nothing" or "Relic", etc.
    struct Room* north;
    struct Room* south;
    struct Room* east;
    struct Room* west;
} Room;

// Allocates and starts a room 
Room* create_room(const char* name, const char* description, const char* item);

// Printed description and available directions
void describe_room(Room* room);

// Optional: Only include if implemented
void connect_rooms(Room* from, Room* to, char direction);

#endif // DUNGEONROOM_H
