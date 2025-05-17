/*
    Game Logic Server for MUD (Final Production Version)
    --------------------------------------------------
    ***Reference***
   
    If you want to update exe:
    Compile with: gcc -o dungeon dungeon.c -lmosquitto


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
#include "map.h"         // Contains our map and movement functions.
#include "connector.h"   // Contains get_next_map().

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345

// Global MQTT settings.
const char* MQTT_BROKER = "GCP IP";  // Update this with your broker IP.
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC_ROOM = "dungeon/room";

struct mosquitto *mosq = NULL;

void publish_to_mqtt(const char* message) {
    int ret = mosquitto_publish(mosq, NULL, MQTT_TOPIC_ROOM, strlen(message),
                                message, 0, false);
    if(ret != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error publishing MQTT: %d\n", ret);
    }
}

int main() {
    mosquitto_lib_init();
    mosq = mosquitto_new(NULL, true, NULL);
    if(!mosq) {
        fprintf(stderr, "Error: Could not create mosquitto instance.\n");
        return 1;
    }
    
    if(mosquitto_connect(mosq, MQTT_BROKER, MQTT_PORT, 60) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: Could not connect to MQTT broker.\n");
        mosquitto_destroy(mosq);
        return 1;
    }
    
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    if(listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL));  // Seed random generator.
    
    int currentMap = MAP1;  // Using MAP1.
    init_map1();            // Initialize map1.
    
    printf("Dungeon Game Logic Server started. Listening on port %d\n", PORT);
    
    while (1) {
        if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        
        char buffer[1024] = {0};
        int valread = read(new_socket, buffer, 1024);
        if(valread <= 0) {
            close(new_socket);
            continue;
        }
        
        printf("Received command: %s\n", buffer);
        char finalMsg[1024] = "";
        int processed = 0;
        
        // Handling connector command (if applicable).
        if(buffer[0] == 'C') {
            int newMap = get_next_map(currentMap);
            currentMap = newMap;
            init_map1(); // For simplicity.
            snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description(currentRoom));
            processed = 1;
        }
        // Handling directional commands.
        else if(buffer[0] == 'n' || buffer[0] == 'e' || buffer[0] == 's' || buffer[0] == 'w') {
            int oldRoom = currentRoom;
            int attemptedRoom = move_room(currentRoom, buffer[0]);
            if(attemptedRoom != currentRoom) {
                currentRoom = attemptedRoom;
                snprintf(finalMsg, sizeof(finalMsg), "%s", get_room_description(currentRoom));
                printf("Moving from %s to %s\n", map1_rooms[oldRoom].name, map1_rooms[currentRoom].name);
            } else {
                // Movement blocked; get a randomized failure message.
                const char* failMsg = get_fail_message();
                snprintf(finalMsg, sizeof(finalMsg), "%s", failMsg);
                printf("Movement blocked: %s\n", failMsg);
            }
            processed = 1;
        }
        
        if(processed) {
            publish_to_mqtt(finalMsg);
            send(new_socket, finalMsg, strlen(finalMsg), 0);
        }
        
        close(new_socket);
        mosquitto_loop(mosq, 10, 1);
    }
    
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 0;
}
