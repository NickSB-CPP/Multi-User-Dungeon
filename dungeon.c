/*
    Game Logic Server for MUD (Final Production Version)
    --------------------------------------------------
    ***Reference***
   
    If you want to update exe:
    Compile with: gcc -o dungeon dungeon.c map1.c map2.c map3.c connector.c -lmosquitto


    This server listens on TCP port 12345 for movement commands (n, s, e, w)
    from a client. Make sure you are listed at the right port to make it work
    the processes of the dungeon logic and sends back room descriptions to work
    via the TCP socket.
   
    It also publishes the room description to the MQTT topic
    "dungeon/room" using the Mosquitto library.
   
    In addition, it prints:
      - The move received on the terminal, so you can check the whole description,
      - The current room (ID and description)
        to the terminal.




    What is this file?
    The game logic, right now making different maps is WIP
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mosquitto.h>
#include <time.h>
#include "map.h"
#include "connector.h"

// Define global room variables only here.
int currentRoom1 = 0;
int currentRoom2 = 0;
int currentRoom3 = 0;

// Global active map variable.
int activeMap = MAP1;

#define TOTAL_MAPS 3

// Global MQTT settings.
const char* MQTT_BROKER = "GCP IP";  // Replace with your actual broker address.
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC_ROOM = "dungeon/room";

struct mosquitto *mosq = NULL;

// Publish message via MQTT.
void publish_to_mqtt(const char* message) {
    int ret = mosquitto_publish(mosq, NULL, MQTT_TOPIC_ROOM, strlen(message), message, 0, false);
    if(ret != MOSQ_ERR_SUCCESS)
        fprintf(stderr, "Error publishing MQTT: %d\n", ret);
}

// External map function declarations.
extern void init_map1();
extern const char* get_room_description1(int);
extern int move_room1(int, char);
extern const char* get_fail_message1();

extern void init_map2();
extern const char* get_room_description2(int);
extern int move_room2(int, char);
extern const char* get_fail_message2();

extern void init_map3();
extern const char* get_room_description3(int);
extern int move_room3(int, char);
extern const char* get_fail_message3();

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345

int main(void) {
    mosquitto_lib_init();
    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        fprintf(stderr, "Error: Could not create mosquitto instance.\n");
        return 1;
    }
    if (mosquitto_connect(mosq, MQTT_BROKER, MQTT_PORT, 60) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: Could not connect to MQTT broker.\n");
        mosquitto_destroy(mosq);
        return 1;
    }
    
    // Setup TCP server.
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL));
    
    // Initialize with MAP1.
    activeMap = MAP1;
    init_map1();
    currentRoom1 = 0;
    
    printf("Dungeon Game Logic Server started. Listening on port %d\n", PORT);
    
    char finalMsg[1024] = "";
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        char buffer[1024] = {0};
        int valread = read(new_socket, buffer, 1024);
        if (valread <= 0) {
            close(new_socket);
            continue;
        }
        
        printf("Received command: %s\n", buffer);
        
        // Process directional command.
        if (buffer[0]=='n' || buffer[0]=='e' || buffer[0]=='s' || buffer[0]=='w') {
            if (activeMap == MAP1) {
                int attemptedRoom = move_room1(currentRoom1, buffer[0]);
                if (attemptedRoom != currentRoom1) {
                    currentRoom1 = attemptedRoom;
                    // If MAP1 connector room is reached (assumed index 9), switch maps.
                    if (currentRoom1 == 9) {
                        printf("MAP1 connector reached. Switching to next map...\n");
                        activeMap = (activeMap + 1) % TOTAL_MAPS;
                        if (activeMap == MAP2) {
                            init_map2(); currentRoom2 = 0;
                            snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description2(0));
                            printf("Switched to MAP2: Starting at Room 0\n");
                        } else if (activeMap == MAP3) {
                            init_map3(); currentRoom3 = 0;
                            snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description3(0));
                            printf("Switched to MAP3: Starting at Room 0\n");
                        }
                    } else {
                        snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description1(currentRoom1));
                        printf("MAP1: Moved to %s\n", map1_rooms[currentRoom1].name);
                    }
                } else {
                    snprintf(finalMsg, sizeof(finalMsg), "%s", get_fail_message1());
                    printf("MAP1: Move blocked. Message: %s\n", finalMsg);
                }
            } else if (activeMap == MAP2) {
                int attemptedRoom = move_room2(currentRoom2, buffer[0]);
                if (attemptedRoom != currentRoom2) {
                    currentRoom2 = attemptedRoom;
                    // For MAP2, connector room is assumed at index 6.
                    if (currentRoom2 == 6) {
                        printf("MAP2 connector reached. Switching to next map...\n");
                        activeMap = (activeMap + 1) % TOTAL_MAPS;
                        if (activeMap == MAP3) {
                            init_map3(); currentRoom3 = 0;
                            snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description3(0));
                            printf("Switched to MAP3: Starting at Room 0\n");
                        } else if (activeMap == MAP1) {
                            init_map1(); currentRoom1 = 0;
                            snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description1(0));
                            printf("Switched to MAP1: Starting at Room 0\n");
                        }
                    } else {
                        snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description2(currentRoom2));
                        printf("MAP2: Moved to Room %d\n", currentRoom2);
                    }
                } else {
                    snprintf(finalMsg, sizeof(finalMsg), "%s", get_fail_message2());
                    printf("MAP2: Move blocked. Message: %s\n", finalMsg);
                }
            } else if (activeMap == MAP3) {
                int attemptedRoom = move_room3(currentRoom3, buffer[0]);
                if (attemptedRoom != currentRoom3) {
                    currentRoom3 = attemptedRoom;
                    // For MAP3, connector room is assumed at index 2.
                    if (currentRoom3 == 2) {
                        printf("MAP3 connector reached. Switching to next map...\n");
                        activeMap = (activeMap + 1) % TOTAL_MAPS;
                        if (activeMap == MAP1) {
                            init_map1(); currentRoom1 = 0;
                            snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description1(0));
                            printf("Switched to MAP1: Starting at Room 0\n");
                        } else if (activeMap == MAP2) {
                            init_map2(); currentRoom2 = 0;
                            snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description2(0));
                            printf("Switched to MAP2: Starting at Room 0\n");
                        }
                    } else {
                        snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description3(currentRoom3));
                        printf("MAP3: Moved to Room %d\n", currentRoom3);
                    }
                } else {
                    snprintf(finalMsg, sizeof(finalMsg), "%s", get_fail_message3());
                    printf("MAP3: Move blocked. Message: %s\n", finalMsg);
                }
            }
        }
        
        printf("Final Message: %s\n", finalMsg);
        publish_to_mqtt(finalMsg);
        send(new_socket, finalMsg, strlen(finalMsg), 0);
        close(new_socket);
        mosquitto_loop(mosq, 10, 1);
    }
    
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 0;
}
