// Description: A simple multi-user dungeon game server using TCP and MQTT.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mosquitto.h>
#include <errno.h>

#define PORT 12345
#define MQTT_BROKER "35.188.102.198"  // Replace with your GCP external IP
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
        "You awaken on the cold, unforgiving stone floor of a dank prison cell," 
        "your head throbbing and your memories clouded. As you start to regain your senses, you smell a stinky smell."
        "You realise that it's the smell of a dead corpse and you notice the corpse in the corner of the room"
        "You see tallies on the walls and you realise your days are numbered.");
    dungeon[0].north = 1;
    dungeon[0].east  = -1;
    dungeon[0].south = 4;
    dungeon[0].west  = -1;
    
    // Room 1: Normal Room.
    dungeon[1].id = 1;
    dungeon[1].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[1].description,
      "A single chair is bolted to the floor under a hanging light. Dried stains mark the floor, 
      and a broken one-way mirror reveals a small, hidden room behind the wall. Echoes of past screams seem to linger unnaturally.");
    dungeon[1].north = -1;
    dungeon[1].east  = 2;
    dungeon[1].south = 0;
    dungeon[1].west  = 9;
    
    // Room 9: Connector Room.
    dungeon[9].id = 9;
    dungeon[9].type = ROOM_TYPE_CONNECTOR;
    strcpy(dungeon[9].description,
      "Connecting");
    dungeon[9].north = -1;
    dungeon[9].east  = 1;
    dungeon[9].south = -1;
    dungeon[9].west  = -1;
    
    // Room 2: Normal Room.
    dungeon[2].id = 2;
    dungeon[2].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[2].description,
      "You find youself in what seems to be a medical ward. Cabinets are open, but most supplies are missing. 
      A patient's journal is stained with ink and blood, ending mid-sentence.");
    dungeon[2].north = -1;
    dungeon[2].east  = 5;
    dungeon[2].south = 3;
    dungeon[2].west  = 1;
    
    // Room 3: Normal Room.
    dungeon[3].id = 3;
    dungeon[3].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[3].description,
      "You find yourself in a solitary confinement room. The walls are bare white and the air is stale. "
      "A single light flickers above, casting eerie shadows.");
    dungeon[3].north = 2;
    dungeon[3].east  = -1;
    dungeon[3].south = 4;
    dungeon[3].west  = -1;
    
    // Room 4: Normal Room.
    dungeon[4].id = 4;
    dungeon[4].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[4].description,
      "A charred electric chair sits under a broken spotlight. The walls are lined with tally marks and old graffiti. 
      A burnt smell lingers in the air, and the switch still clicks ominously when flipped");
    dungeon[4].north = 3;
    dungeon[4].east  = -1;
    dungeon[4].south = 0;
    dungeon[4].west  = -1;
    
    // Room 5: Normal Room.
    dungeon[5].id = 5;
    dungeon[5].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[5].description,
      "Lavish compared to the rest of the prison, this room is dusty but intact. A desk, a locked safe, and a family portrait are here. 
      A false floorboard creaks under pressure, and a hidden door is outlined in the back wall.");
    dungeon[5].north = -1;
    dungeon[5].east  = -1;
    dungeon[5].south = 6;
    dungeon[5].west  = 2;
    
    // Room 6: Normal Room.
    dungeon[6].id = 6;
    dungeon[6].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[6].description,
      "Mold creeps up the tiled walls. The sound of dripping water echoes endlessly. The pipes groan under pressure, 
      and sometimes spray scalding steam without warning. A rusted drain cover hides something trapped beneath it.");
    dungeon[6].north = 5;
    dungeon[6].east  = -1;
    dungeon[6].south = 7;
    dungeon[6].west  = -1;
    
    // Room 7: Normal Room.
    dungeon[7].id = 7;
    dungeon[7].type = ROOM_TYPE_NORMAL;
    strcpy(dungeon[7].description,
      "Sweltering heat and hissing steam fill this chamber. The machinery is mostly defunct, 
       except for one gauge that still twitches. An old maintenance journal ends with: “They never left.”");
    dungeon[7].north = -1;
    dungeon[7].east  = -1;
    dungeon[7].south = -1;
    dungeon[7].west  = 8;
    
    // Room 8: Treasure Room.
    dungeon[8].id = 8;
    dungeon[8].type = ROOM_TYPE_TREASURE;
    strcpy(dungeon[8].description,
      "The chairs and phones are intact, but dust-covered. One phone receiver is off the hook, 
      and if picked up, a voice whispers faintly. The logbook has a name circled repeatedly with the word “DENIED.”");
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