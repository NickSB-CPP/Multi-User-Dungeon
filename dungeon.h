#ifndef DUNGEON_H
#define DUNGEON_H

typedef enum {
    ROOM_TYPE_START,
    ROOM_TYPE_TREASURE,
    ROOM_TYPE_NORMAL,
    ROOM_TYPE_CONNECTOR
} RoomType;

typedef struct Room {
    int id;
    RoomType type;
    char description[512];
    int north;
    int east;
    int south;
    int west;
} Room;

#endif