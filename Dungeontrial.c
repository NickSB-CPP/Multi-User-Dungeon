#include "dungeon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mosquitto.h>
#include <errno.h>

#define PORT 12345
#define MQTT_BROKER "35.194.16.121"
#define MQTT_PORT 1883
#define MQTT_TOPIC "dungeon/room"

extern void setupRoomsA(Room dungeon[]);
extern void setupRoomsB(Room dungeon[]);
extern void setupRoomsC(Room dungeon[]);
extern void setupRoomsD(Room dungeon[]);

void connectRooms(Room* dungeon, int from, char dir, int to) {
    switch (dir) {
        case 'n': dungeon[from].north = to; dungeon[to].south = from; break;
        case 'e': dungeon[from].east  = to; dungeon[to].west  = from; break;
        case 's': dungeon[from].south = to; dungeon[to].north = from; break;
        case 'w': dungeon[from].west  = to; dungeon[to].east  = from; break;
    }
}

void setupDungeon(Room dungeon[]) {
    setupRoomsA(dungeon);
    setupRoomsB(dungeon);
    setupRoomsC(dungeon);
    setupRoomsD(dungeon);
    connectRooms(dungeon, 1, 'e', 10);
    connectRooms(dungeon, 10, 'e', 11);
}

int main(void) {
srand((unsigned int)time(NULL));
Room dungeon[100];
setupDungeon(dungeon);

// Randomize starting room
int startingOptions[] = {0, 10, 20, 30};
int currentRoom = startingOptions[rand() % 4];
printf("Starting in Room %d: %s\n", currentRoom, dungeon[currentRoom].description);

// Randomize portal destinations without duplicates
int startRooms[] = {0, 10, 20, 30};
for (int i = 3; i > 0; --i) {
    int j = rand() % (i + 1);
    int temp = startRooms[i];
    startRooms[i] = startRooms[j];
    startRooms[j] = temp;
}
int portalB_dest = startRooms[0];  // Portal from Room 19
int portalC_dest = startRooms[1];  // Portal from Room 29
int portalD_dest = startRooms[2];  // Portal from Room 35

printf("Portal 19 → %d\n", portalB_dest);
printf("Portal 29 → %d\n", portalC_dest);
printf("Portal 35 → %d\n", portalD_dest);

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

// Auto-teleport when entering connector portal rooms
if (currentRoom == 19) {
    currentRoom = portalB_dest;
    printf("Teleported from Room 19 → %d: %s\n", currentRoom, dungeon[currentRoom].description);
    goto SEND_DESCRIPTION;
}
if (currentRoom == 29) {
    currentRoom = portalC_dest;
    printf("Teleported from Room 29 → %d: %s\n", currentRoom, dungeon[currentRoom].description);
    goto SEND_DESCRIPTION;
}
if (currentRoom == 35) {
    currentRoom = portalD_dest;
    printf("Teleported from Room 35 → %d: %s\n", currentRoom, dungeon[currentRoom].description);
    goto SEND_DESCRIPTION;
}



        int nextRoom = currentRoom;
        switch (command) {
            case 'n': nextRoom = dungeon[currentRoom].north; break;
            case 's': nextRoom = dungeon[currentRoom].south; break;
            case 'e': nextRoom = dungeon[currentRoom].east; break;
            case 'w': nextRoom = dungeon[currentRoom].west; break;
            case 'q': quit = 1; continue;
            default: continue;
        }

        if (nextRoom == -1) {
            const char *msg = "No path in that direction!";
            send(client_fd, msg, strlen(msg), 0);
            mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(msg), msg, 0, false);
        } else {
    currentRoom = nextRoom;
    printf("Entered Room %d: %s\n", currentRoom, dungeon[currentRoom].description);

SEND_DESCRIPTION:
            char buffer[1024];
            snprintf(buffer, sizeof(buffer), "%s\n", dungeon[currentRoom].description);
            send(client_fd, buffer, strlen(buffer), 0);
            mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(buffer), buffer, 0, false);

            if (dungeon[currentRoom].type == ROOM_TYPE_TREASURE) {
                const char *winMsg = "Treasure found! Game Over.";
                send(client_fd, winMsg, strlen(winMsg), 0);
                mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(winMsg), winMsg, 0, false);
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
