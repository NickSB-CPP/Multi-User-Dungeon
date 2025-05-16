#include <stdio.h>
#include <stdlib.h>
#include "room1.h"
#include "room2.h"
#include "room3.h"
#include "room4.h"
#include "room.h"  // for describe_room(), etc.

int main(void) {
    // Each Group, 10 Rooms
    Room** g1 = create_group1_rooms(); // group 1 (me)
    Room** g2 = create_group2_rooms(); // group 2 (partner 1)
    Room** g3 = create_group3_rooms(); // group 3 (partner 2)
    Room** g4 = create_group4_rooms(); // group 4 (partner 3)

    // Connect rooms between groups
    g1[9]->east = g2[0]; g2[0]->west = g1[9];
    g2[9]->south = g3[0]; g3[0]->north = g2[9];
    g3[9]->east = g4[0]; g4[0]->west = g3[9];
    g4[9]->north = g1[0]; g1[0]->south = g4[9];

    // Start in room 0 of group 1
    Room* current = g1[0];
    char command;

    while (1) {
        describe_room(current);
        printf("\nMove (W=North, S=South, D=East, A=West, Q=Quit): ");
        scanf(" %c", &command);

        switch (command) {
            case 'W': case 'w':
                if (current->north) current = current->north;
                else printf("Blocked.\n");
                break;
            case 'S': case 's':
                if (current->south) current = current->south;
                else printf("Blocked.\n");
                break;
            case 'A': case 'a':
                if (current->west) current = current->west;
                else printf("Blocked.\n");
                break;
            case 'D': case 'd':
                if (current->east) current = current->east;
                else printf("Blocked.\n");
                break;
            case 'Q': case 'q':
                printf("Quitting...\n");
                return 0;
            default:
                printf("Invalid input.\n");
        }
    }
}
