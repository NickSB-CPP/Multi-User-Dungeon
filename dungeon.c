/*
    Game Logic Server for MUD (Final Production Version)
    --------------------------------------------------
    Updated to randomize map selection at connectors and to remember previous states
    so that you can back track exactly.
    
    Compile with:
      gcc -o dungeon dungeon.c map1.c map2.c map3.c connector.c -lmosquitto

    This server listens on TCP port 12345 for movement commands (n, s, e, w)
    from a client. When the player enters a connector room, the next map is chosen 
    at random (if no history exists) or a previous map state is restored (if you have backtracked).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mosquitto.h>
#include <time.h>
#include "map.h"
#include "connector.h"

// Global room variables.
int currentRoom1 = 0;
int currentRoom2 = 0;
int currentRoom3 = 0;

// Global active map variable.
int activeMap = MAP1;
#define TOTAL_MAPS 3

// Global MQTT settings.
const char* MQTT_BROKER = "";  // Replace with your actual broker address.
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC_ROOM = "dungeon/room";

struct mosquitto *mosq = NULL;

/*--- History stack for backtracking ---*/
typedef struct {
    int map;    // The previous map ID.
    int room;   // The room (in that map) from which you left.
} MapState;

#define HISTORY_MAX 100
MapState history[HISTORY_MAX];
int historyTop = 0;  // index of next free slot

// Push a state onto the history stack.
void pushHistory(int map, int room) {
    if(historyTop < HISTORY_MAX) {
        history[historyTop].map = map;
        history[historyTop].room = room;
        historyTop++;
    } else {
        fprintf(stderr, "History overflow!\n");
    }
}

// Pop a state from the history stack. Returns 1 if successful; 0 if empty.
int popHistory(MapState *state) {
    if(historyTop > 0) {
        historyTop--;
        *state = history[historyTop];
        return 1;
    }
    return 0;
}

/*
 * Returns a random map id in [0, TOTAL_MAPS-1] that is different from currentMap.
 */
int get_random_map(int currentMap) {
    int newMap;
    do {
        newMap = rand() % TOTAL_MAPS;
    } while (newMap == currentMap);
    return newMap;
}

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
    
    // Start in MAP1.
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
        // Process the directional command: n, s, e, or w.
        if (buffer[0]=='n' || buffer[0]=='e' || buffer[0]=='s' || buffer[0]=='w') {
            // --- MAP1 branch ---
            if (activeMap == MAP1) {
                int attemptedRoom = move_room1(currentRoom1, buffer[0]);
                if (attemptedRoom != currentRoom1) {
                    currentRoom1 = attemptedRoom;
                    // Check for connector rooms:
                    if (currentRoom1 == 9) {  
                        // Forward connector in MAP1.
                        printf("MAP1 forward connector reached.\n");
                        pushHistory(MAP1, 9);  // Remember that you left MAP1 at room9.
                        int newMap = get_random_map(MAP1);
                        if (newMap == MAP2) {
                            init_map2(); currentRoom2 = 0; activeMap = MAP2;
                            snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description2(0));
                            printf("Switched to MAP2: Starting at Room 0\n");
                        } else if (newMap == MAP3) {
                            init_map3(); currentRoom3 = 0; activeMap = MAP3;
                            snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description3(0));
                            printf("Switched to MAP3: Starting at Room 0\n");
                        }
                    } 
                    else if (currentRoom1 == 0 && historyTop > 0) {  
                        // In MAP1, room0 is designated for backward movement.
                        MapState prev;
                        if(popHistory(&prev)) {
                            printf("MAP1 backward connector reached. Backtracking...\n");
                            if (prev.map == MAP1) {
                                init_map1(); currentRoom1 = prev.room; activeMap = MAP1;
                                snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description1(prev.room));
                            } else if (prev.map == MAP2) {
                                init_map2(); currentRoom2 = prev.room; activeMap = MAP2;
                                snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description2(prev.room));
                            } else if (prev.map == MAP3) {
                                init_map3(); currentRoom3 = prev.room; activeMap = MAP3;
                                snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description3(prev.room));
                            }
                        }
                    } 
                    else {
                        // Normal move in MAP1.
                        snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description1(currentRoom1));
                        printf("MAP1: Moved to %s\n", map1_rooms[currentRoom1].name);
                    }
                } else {
                    snprintf(finalMsg, sizeof(finalMsg), "%s", get_fail_message1());
                    printf("MAP1: Move blocked. Message: %s\n", finalMsg);
                }
            }
            // --- MAP2 branch ---
            else if (activeMap == MAP2) {
                int attemptedRoom = move_room2(currentRoom2, buffer[0]);
                if (attemptedRoom != currentRoom2) {
                    currentRoom2 = attemptedRoom;
                    if (currentRoom2 == 6) {  
                        // Forward connector in MAP2.
                        printf("MAP2 forward connector reached.\n");
                        pushHistory(MAP2, 6);
                        int newMap = get_random_map(MAP2);
                        if (newMap == MAP1) {
                            init_map1(); currentRoom1 = 0; activeMap = MAP1;
                            snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description1(0));
                            printf("Switched to MAP1: Starting at Room 0\n");
                        } else if (newMap == MAP3) {
                            init_map3(); currentRoom3 = 0; activeMap = MAP3;
                            snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description3(0));
                            printf("Switched to MAP3: Starting at Room 0\n");
                        }
                    } 
                    else if (currentRoom2 == 0 && historyTop > 0) {  
                        // In MAP2, room0 is the backward connector.
                        MapState prev;
                        if(popHistory(&prev)) {
                            printf("MAP2 backward connector reached. Backtracking...\n");
                            if(prev.map == MAP1) {
                                init_map1(); currentRoom1 = prev.room; activeMap = MAP1;
                                snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description1(prev.room));
                            } else if(prev.map == MAP2) {
                                init_map2(); currentRoom2 = prev.room; activeMap = MAP2;
                                snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description2(prev.room));
                            } else if(prev.map == MAP3) {
                                init_map3(); currentRoom3 = prev.room; activeMap = MAP3;
                                snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description3(prev.room));
                            }
                        }
                    } 
                    else {
                        snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description2(currentRoom2));
                        printf("MAP2: Moved to Room %d\n", currentRoom2);
                    }
                } else {
                    snprintf(finalMsg, sizeof(finalMsg), "%s", get_fail_message2());
                    printf("MAP2: Move blocked. Message: %s\n", finalMsg);
                }
            }
            // --- MAP3 branch ---
            else if (activeMap == MAP3) {
                int attemptedRoom = move_room3(currentRoom3, buffer[0]);
                if (attemptedRoom != currentRoom3) {
                    currentRoom3 = attemptedRoom;
                    if (currentRoom3 == 2) {  
                        // Forward connector in MAP3.
                        printf("MAP3 forward connector reached.\n");
                        pushHistory(MAP3, 2);
                        int newMap = get_random_map(MAP3);
                        if (newMap == MAP1) {
                            init_map1(); currentRoom1 = 0; activeMap = MAP1;
                            snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description1(0));
                            printf("Switched to MAP1: Starting at Room 0\n");
                        } else if (newMap == MAP2) {
                            init_map2(); currentRoom2 = 0; activeMap = MAP2;
                            snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description2(0));
                            printf("Switched to MAP2: Starting at Room 0\n");
                        }
                    }
                    else if (currentRoom3 == 0 && historyTop > 0) {  
                        // In MAP3, room0 is the backward connector.
                        MapState prev;
                        if(popHistory(&prev)) {
                            printf("MAP3 backward connector reached. Backtracking...\n");
                            if(prev.map == MAP1) {
                                init_map1(); currentRoom1 = prev.room; activeMap = MAP1;
                                snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description1(prev.room));
                            } else if(prev.map == MAP2) {
                                init_map2(); currentRoom2 = prev.room; activeMap = MAP2;
                                snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description2(prev.room));
                            } else if(prev.map == MAP3) {
                                init_map3(); currentRoom3 = prev.room; activeMap = MAP3;
                                snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description3(prev.room));
                            }
                        }
                    }
                    else {
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
