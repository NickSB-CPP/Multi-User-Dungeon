/*
   Game Logic Server for MUD (Final Production Version)
   --------------------------------------------------
   Compile with: gcc -o dungeon dungeon.c -lmosquitto

   This server listens on TCP port 12345 for movement commands (n, s, e, w)
   from a client. It processes the dungeon logic and sends back room descriptions
   via the TCP socket. It also publishes the room description to the MQTT topic
   "dungeon/room" using the Mosquitto library. In addition, it prints:
     - The move received,
     - The current room (ID and description)
   to the terminal.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mosquitto.h>
#include <errno.h>

#define PORT 12345
#define MQTT_BROKER "34.70.43.249"  // Replace with your GCP external IP
#define MQTT_PORT 1883
#define MQTT_TOPIC "dungeon/room"

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

// Set up the dungeon layout (10 rooms).
void setupDungeon(Room dungeon[]) {
    // Room 0: Start Room.
    dungeon[0].id = 0;
    dungeon[0].type = ROOM_TYPE_START;
    strcpy(dungeon[0].description,
      "In a cramped, dark room, all you can feel is the stone walls around you. The air thickens, distant clanks of metal and footsteps echo in the distance.");
    dungeon[0].north = 1;
    dungeon[0].east  = -1;
    dungeon[0].south = 4;
    dungeon[0].west  = -1;
    
    // Room 1: Normal Room.
    dungeon[1].id = 1;
    dungeon[1].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[1].description,
      "You step outside, you can feel the breeze. Despite that, no light is present. Shuffling around, you find you are surrounded by that same stone walls and an arrow on a target.");
    dungeon[1].north = -1;
    dungeon[1].east  = 2;
    dungeon[1].south = 0;
    dungeon[1].west  = 9;
    
    // Room 9: Connector Room.
    dungeon[9].id = 9;
    dungeon[9].type = ROOM_TYPE_CONNECTOR;
    dungeon[9].north = -1;
    dungeon[9].east  = 1;
    dungeon[9].south = -1;
    dungeon[9].west  = -1;
    
    // Room 2: Normal Room.
    dungeon[2].id = 2;
    dungeon[2].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[2].description,
      "You step out and feel a chair and a table. Putting your hand and moving across the table, it is incredibly long, as if it was made for an army. Where is everyone?");
    dungeon[2].north = -1;
    dungeon[2].east  = 5;
    dungeon[2].south = 3;
    dungeon[2].west  = 1;
    
    // Room 3: Normal Room.
    dungeon[3].id = 3;
    dungeon[3].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[3].description,
      "Into a corridor you go. Moving your hand along the wall, you feel paintings. You retract it to not potentially damage them. ");
    dungeon[3].north = 2;
    dungeon[3].east  = -1;
    dungeon[3].south = 4;
    dungeon[3].west  = -1;
    
    // Room 4: Normal Room.
    dungeon[4].id = 4;
    dungeon[4].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[4].description,
      "You wander onto a twisting trail where trees seem to conspire together. The path eventually circles back to the clearing.");
    dungeon[4].north = 3;
    dungeon[4].east  = -1;
    dungeon[4].south = 0;
    dungeon[4].west  = -1;
    
    // Room 5: Normal Room.
    dungeon[5].id = 5;
    dungeon[5].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[5].description,
      "Venturing through a secret exit, you discover a hidden trail shimmering with golden luminescence. Only the intrepid may continue.");
    dungeon[5].north = -1;
    dungeon[5].east  = -1;
    dungeon[5].south = 6;
    dungeon[5].west  = 2;
    
    // Room 6: Normal Room.
    dungeon[6].id = 6;
    dungeon[6].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[6].description,
      "The hidden path narrows into a dim corridor where even moonbeams struggle to penetrate. Gnarled roots and scattered stones make each step cautious.");
    dungeon[6].north = 5;
    dungeon[6].east  = -1;
    dungeon[6].south = 7;
    dungeon[6].west  = -1;
    
    // Room 7: Normal Room.
    dungeon[7].id = 7;
    dungeon[7].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[7].description,
      "Emerging into a secluded glade, you sense a mysterious magic. A solitary exit to the west hints at a secret domain.");
    dungeon[7].north = -1;
    dungeon[7].east  = -1;
    dungeon[7].south = -1;
    dungeon[7].west  = 8;
    
    // Room 8: Treasure Room.
    dungeon[8].id = 8;
    dungeon[8].type = ROOM_TYPE_TREASURE;
    strcpy(dungeon[8].description,
      "Ancient relics, weapons, gold... As you approach the room, you finally see light... As the moon shines upon all the metal before you... Your journey is finally over.");
    dungeon[8].north = -1;
    dungeon[8].east  = -1;
    dungeon[8].south = -1;
    dungeon[8].west  = -1;
}

int main(void) {
    srand((unsigned int)time(NULL));
    Room dungeon[10];
    setupDungeon(dungeon);
    int currentRoom = 0;  // Start in Room 0

    // --- MQTT Initialization ---
    mosquitto_lib_init();
    struct mosquitto *mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        fprintf(stderr, "Error: Could not create MQTT instance.\n");
        return 1;
    }
    if (mosquitto_connect(mosq, MQTT_BROKER, MQTT_PORT, 60) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: Could not connect to MQTT Broker at %s:%d.\n", MQTT_BROKER, MQTT_PORT);
        mosquitto_destroy(mosq);
        return 1;
    }

    // --- Setup TCP Socket ---
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return 1;
    }
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        return 1;
    }
    printf("Server listening on port %d...\n", PORT);
    fflush(stdout);
    
    printf("Waiting for a client to connect...\n");
    fflush(stdout);
    
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        perror("Accept failed");
        close(server_fd);
        return 1;
    }
    printf("Client connected.\n");
    fflush(stdout);

    char command;
    int quit = 0;
    while (!quit) {
        int bytes_received = recv(client_fd, &command, 1, 0);
        if (bytes_received <= 0) {
            perror("Error reading from client or client disconnected");
            break;
        }
        // Print which move was received.
        printf("Move received: %c\n", command);
        fflush(stdout);

        int nextRoom = currentRoom;
        switch (command) {
            case 'n': case 'N':
                nextRoom = dungeon[currentRoom].north;
                break;
            case 's': case 'S':
                nextRoom = dungeon[currentRoom].south;
                break;
            case 'e': case 'E':
                nextRoom = dungeon[currentRoom].east;
                break;
            case 'w': case 'W':
                nextRoom = dungeon[currentRoom].west;
                break;
            case 'q': case 'Q':
                quit = 1;
                break;
            default:
                break;
        }
        if (nextRoom == -1) {
            const char *msg = "No path in that direction!";
            send(client_fd, msg, strlen(msg), 0);
            mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(msg), msg, 0, false);
            printf("Room message: %s\n", msg);
            fflush(stdout);
        } else {
            currentRoom = nextRoom;
            char buffer[1024];
            snprintf(buffer, sizeof(buffer), "%s\n", dungeon[currentRoom].description);
            send(client_fd, buffer, strlen(buffer), 0);
            mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(buffer), buffer, 0, false);
            // Print the room description in the terminal.
            printf("Entered Room %d: %s", dungeon[currentRoom].id, buffer);
            fflush(stdout);
            if (dungeon[currentRoom].type == ROOM_TYPE_TREASURE) {
                const char *winMsg = "Treasure found! Game Over.";
                send(client_fd, winMsg, strlen(winMsg), 0);
                mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(winMsg), winMsg, 0, false);
                printf("Room message: %s\n", winMsg);
                fflush(stdout);
                break;
            }
        }
    }

    close(client_fd);
    close(server_fd);
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 0;
}
