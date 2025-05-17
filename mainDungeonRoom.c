#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "room.h"

#define ROOM_COUNT 10

Room* build_world(Room** winningRoom) {
    Room* rooms[ROOM_COUNT];

    // Create rooms with vague descriptions
    const char* descriptions[ROOM_COUNT] = {
        "The air is still. A faint hum echoes in the dark.",            // Room 0
        "The floor is made of smooth stone, slightly damp.",            // Room 1
        "Something scratches in the walls... or is it just wind?",      // Room 2
        "A faint light glows behind the cracks.",                       // Room 3
        "It smells like moss and cold metal.",                          // Room 4
        "You feel watched. There's nothing around, or is there?",       // Room 5
        "Shadows ripple across the walls.",                             // Room 6
        "It's warm here. Too warm.",                                    // Room 7
        "You step on something soft. It moves.",                        // Room 8
        "A strange energy surrounds you... you've found something!"     // Room 9
    };

    for (int i = 0; i < ROOM_COUNT; i++) {
        rooms[i] = create_room(descriptions[i]);
    }

    // Connect rooms according to your layout
    connect_rooms(rooms[0], rooms[1], 'S');
    connect_rooms(rooms[1], rooms[0], 'N');
    connect_rooms(rooms[1], rooms[2], 'S');
    connect_rooms(rooms[1], rooms[3], 'E');
    connect_rooms(rooms[2], rooms[1], 'W');
    connect_rooms(rooms[2], rooms[4], 'E');
    connect_rooms(rooms[2], rooms[5], 'S');
    connect_rooms(rooms[3], rooms[1], 'W');
    connect_rooms(rooms[3], rooms[4], 'E');
    connect_rooms(rooms[4], rooms[2], 'W');
    connect_rooms(rooms[4], rooms[5], 'S');
    connect_rooms(rooms[4], rooms[6], 'E');
    connect_rooms(rooms[5], rooms[2], 'N');
    connect_rooms(rooms[5], rooms[6], 'E');
    connect_rooms(rooms[5], rooms[8], 'W');
    connect_rooms(rooms[5], rooms[7], 'S');
    connect_rooms(rooms[6], rooms[4], 'W');
    connect_rooms(rooms[6], rooms[5], 'S');
    connect_rooms(rooms[6], rooms[7], 'N');
    connect_rooms(rooms[7], rooms[5], 'N');
    connect_rooms(rooms[7], rooms[6], 'W');
    connect_rooms(rooms[8], rooms[5], 'E');
    connect_rooms(rooms[8], rooms[9], 'S'); // -> Winning path
    connect_rooms(rooms[9], NULL, 'N'); // End room, no exits

    *winningRoom = rooms[9]; // Return winning room pointer
    return rooms[0]; // Start at Room 0
}

int main() {
    char input;
    Room* currentRoom;
    Room* winningRoom;

    currentRoom = build_world(&winningRoom);

    while (1) {
        describe_room(currentRoom);
        printf("Move with W/A/S/D, or Q to quit: ");
        scanf(" %c", &input);
        input = toupper(input);

        if (currentRoom == winningRoom) {
            printf("\n🎉 Congratulations! You have completed the maze!\n");
            printf("Thanks for playing!\n");
            printf("Press [R] to play again, or [Q] to quit: ");
            scanf(" %c", &input);
            input = toupper(input);
            if (input == 'R') {
                currentRoom = build_world(&winningRoom);
                continue;
            } else {
                break;
            }
        }

        switch (input) {
            case 'W':
                if (currentRoom->north) currentRoom = currentRoom->north;
                else printf("Blocked path.\n");
                break;
            case 'S':
                if (currentRoom->south) currentRoom = currentRoom->south;
                else printf("Blocked path.\n");
                break;
            case 'A':
                if (currentRoom->west) currentRoom = currentRoom->west;
                else printf("Blocked path.\n");
                break;
            case 'D':
                if (currentRoom->east) currentRoom = currentRoom->east;
                else printf("Blocked path.\n");
                break;
            case 'Q':
                printf("Thanks for playing!\n");
                return 0;
            default:
                printf("Invalid input.\n");
        }
    }

    return 0;
}
//Congrats!
