#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Define room types.
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


void setupDungeon(Room dungeon[]) {
    // Room 0: Start Room
    dungeon[0].id = 0;
    dungeon[0].type = ROOM_TYPE_START;
    strcpy(dungeon[0].description,
           "You stand in a tranquil clearing, bathed in a ghostly moonlight. Ancient trees form a natural barrier around you, "
           "their gnarled branches weaving into a mysterious canopy overhead. A soft mist rises from the forest floor, and the distant rustle of leaves sings "
           "of secrets hidden deep within these woods. To the north, a barely discernible path beckons, while a well-worn trail leads to the south.");
    dungeon[0].north = 1;   // to Room 1 | Numbers represents rooms
    dungeon[0].east  = -1; // No room, just a wall
    dungeon[0].south = 4;   // to Room 4
    dungeon[0].west  = -1;
    
    // Room 1
    dungeon[1].id = 1;
    dungeon[1].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[1].description,
           "You step onto a narrow, winding trail. The forest here grows denser; shadows twist among the trunks as if harboring long-forgotten memories. "
           "The path splits: a glimmering light seems to emerge from the east, while the route back to the clearing lies to the south. Off to the west, a peculiar passage "
           "beckons—its immaculate condition suggesting it links to realms beyond these woods.");
    dungeon[1].north = -1;
    dungeon[1].east  = 2;  
    dungeon[1].south = 0;   
    dungeon[1].west  = 9;   
    
    // Room 9: Connector Room for Other Modules
    dungeon[9].id = 9;
    dungeon[9].type = ROOM_TYPE_CONNECTOR;
    strcpy(dungeon[9].description,
           "You find yourself in a narrow corridor lit by an eerie, phosphorescent glow. The walls are cloaked in luminous moss, "
           "and the passage seems to have been deliberately crafted. This well-kept path appears to serve as a gateway—linking these Lost Woods to unexplored territories beyond.");
    dungeon[9].north = -1;
    dungeon[9].east  = 1;   
    dungeon[9].south = -1;
    dungeon[9].west  = -1;
    
    // Room 2
    dungeon[2].id = 2;
    dungeon[2].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[2].description,
           "The forest opens into a small, enchanted grove where moonlight dances upon dew-laden leaves. An inexplicable shimmer in the air draws your eye "
           "to an unassuming exit to the east—a hidden path that whispers of untold discoveries. The familiar trail winds to the south, while the way back to Room 1 offers "
           "a comforting familiarity.");
    dungeon[2].north = -1;
    dungeon[2].east  = 5;   
    dungeon[2].south = 3;   
    dungeon[2].west  = 1;   
    
    // Room 3
    dungeon[3].id = 3;
    dungeon[3].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[3].description,
           "The woods here have grown eerily silent. The narrow path is marked by ancient stone markers, half-consumed by moss and time. Every footstep echoes "
           "with a mysterious resonance, mingling with the soft whispers of unseen presences. A faint trail stretches further south, while the way back north is shrouded in melancholy.");
    dungeon[3].north = 2;   
    dungeon[3].east  = -1;
    dungeon[3].south = 4;  
    dungeon[3].west  = -1;
    
    // Room 4
    dungeon[4].id = 4;
    dungeon[4].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[4].description,
           "You wander onto a twisting trail where the forest seems to conspire in a perpetual loop. Towering trees arch overhead, blocking out starlight "
           "and casting the road in perpetual twilight. An omnipresent hum of ancient magic permeates the air, and despite your efforts, the path inexorably guides you back "
           "to the clearing.");
    dungeon[4].north = 3;   
    dungeon[4].east  = -1;
    dungeon[4].south = 0;   
    dungeon[4].west  = -1;
    

    dungeon[5].id = 5;
    dungeon[5].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[5].description,
           "Venturing through the secret exit in Room 2, you discover a hidden trail unlike any other. Here, the air shimmers with a golden luminescence "
           "and each leaf appears to sparkle with enchantment. The path is scarcely trodden, inviting only the most intrepid souls to uncover the mysteries that lie ahead.");
    dungeon[5].north = -1;
    dungeon[5].east  = -1;
    dungeon[5].south = 6;   
    dungeon[5].west  = 2;  
    
    // Room 6
    dungeon[6].id = 6;
    dungeon[6].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[6].description,
           "The secret trail narrows into a dim corridor where moonbeams barely penetrate the dense canopy. Gnarled roots and scattered stones make each step cautious, "
           "and the silence is broken only by the distant call of unseen creatures. The atmosphere is heavy with the whispers of legends long past, urging you onward.");
    dungeon[6].north = 5;   
    dungeon[6].east  = -1;
    dungeon[6].south = 7;   
    dungeon[6].west  = -1;
    
    // Room 7
    dungeon[7].id = 7;
    dungeon[7].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[7].description,
           "Emerging from the narrow passage, you enter a secluded glade filled with an almost palpable magic. The air is thick with the scent of earth and time, "
           "and the trees seem to lean inward as if in silent reverence. In the west, a solitary exit hints at a secret domain—a final, tantalizing passage that promises extraordinary wonders.");
    dungeon[7].north = -1;
    dungeon[7].east  = -1;
    dungeon[7].south = -1;
    dungeon[7].west  = 8;   
    
    // Room 8
    dungeon[8].id = 8;
    dungeon[8].type = ROOM_TYPE_TREASURE;
    strcpy(dungeon[8].description,
           "You have finally discovered the hidden grove. Before you lies a trove of ancient relics, glittering jewels, and mysterious artifacts "
           "bathed in an ethereal glow. The ambiance vibrates with a sense of accomplishment and wonder—a reward for your perseverance through the enigmatic Lost Woods.");
    dungeon[8].north = -1;
    dungeon[8].east  = -1;
    dungeon[8].south = -1;
    dungeon[8].west  = -1;
}

int main(void) {
    srand((unsigned int)time(NULL));
    
    // set up
    Room dungeon[10];
    setupDungeon(dungeon);
    
    int currentRoom = 0;  // Player starts in Room 0
    char command;
    int quit = 0;
    
    while (!quit) {
        // Display the current room's description.
        printf("\n%s\n", dungeon[currentRoom].description);
        
        // Check for treasure: the winning condition.
        if (dungeon[currentRoom].type == ROOM_TYPE_TREASURE) {
            printf("\nCongratulations! You have uncovered the hidden treasure of the Lost Woods!\n");
            break;
        }
        
        // Prompt the player for a direction.
        printf("\nEnter direction (N, E, S, W) or Q to quit: ");
        
        // Read the first non-space character || Checks for input.
        do {
            command = getchar();
        } while (command == ' ' || command == '\n');
        
        int nextRoom = currentRoom;
        int validCommand = 1;
        
        switch (command) {
            case 'N': case 'n':
                nextRoom = dungeon[currentRoom].north;
                break;
            case 'E': case 'e':
                nextRoom = dungeon[currentRoom].east;
                break;
            case 'S': case 's':
                nextRoom = dungeon[currentRoom].south;
                break;
            case 'W': case 'w':
                nextRoom = dungeon[currentRoom].west;
                break;
            case 'Q': case 'q':
                quit = 1;
                printf("Quitting the dungeon. Farewell!\n");
                break;
            default:
                validCommand = 0;
                printf("Invalid command. Please use N, E, S, W, or Q.\n");
                break;
        }
        
        // Clear any extra characters from input.
        while (getchar() != '\n');
        
        if (!validCommand || quit)
            continue;
        
        if (nextRoom == -1) {
            printf("You wander into impassable thickets where no path can be found.\n");
        } else {
            currentRoom = nextRoom;
        }
    }
    
    return 0;
}
