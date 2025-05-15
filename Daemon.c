// dungeon_daemon.c - GCP Server Daemon for ESP32 Dungeon Game (10 custom rooms with UDP debugging)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <errno.h>
#include <mosquitto.h>

#define PORT 8080
#define MAX_PLAYERS 10
#define BUF_SIZE 128
#define NUM_ROOMS 10

const char *mqtt_host = "localhost";
const int mqtt_port = 1883;

struct Room {
    char *name;
    char *description;
    int north, south, east, west;
};

struct Player {
    char id[16];
    int current_room;
};

struct Room rooms[NUM_ROOMS];
struct Player players[MAX_PLAYERS];
int player_count = 0;

struct mosquitto *mosq;

void init_rooms() {
    rooms[0] = (struct Room){"", 1, -1, -1, -1};
    rooms[1] = (struct Room){"", 2, 0, -1, -1};
    rooms[2] = (struct Room){"", -1, 1, 3, -1};
    rooms[3] = (struct Room){"", -1, -1, 4, 2};
    rooms[4] = (struct Room){"", 5, -1, -1, 3};
    rooms[5] = (struct Room){"", -1, 4, 6, -1};
    rooms[6] = (struct Room){"", -1, -1, 7, 5};
    rooms[7] = (struct Room){"", -1, -1, 8, 6};
    rooms[8] = (struct Room){"", -1, -1, 9, 7};
    rooms[9] = (struct Room){"", -1, -1, -1, 8};
}

int find_or_add_player(const char *id) {
    for (int i = 0; i < player_count; ++i) {
        if (strcmp(players[i].id, id) == 0) return i;
    }
    strncpy(players[player_count].id, id, 15);
    players[player_count].current_room = rand() % NUM_ROOMS;
    return player_count++;
}

void send_room_description(const char *player_id, int room_id) {
    char topic[64];
    snprintf(topic, sizeof(topic), "game/%s/roomdesc", player_id);
    printf("Publishing to topic: %s -> %s\n", topic, rooms[room_id].description); // DEBUG
    mosquitto_publish(mosq, NULL, topic, strlen(rooms[room_id].description), rooms[room_id].description, 0, false);
}

int move_player(int room_id, const char *dir) {
    if (strcmp(dir, "N") == 0 && rooms[room_id].north != -1) return rooms[room_id].north;
    if (strcmp(dir, "S") == 0 && rooms[room_id].south != -1) return rooms[room_id].south;
    if (strcmp(dir, "E") == 0 && rooms[room_id].east != -1) return rooms[room_id].east;
    if (strcmp(dir, "W") == 0 && rooms[room_id].west != -1) return rooms[room_id].west;
    return room_id; // invalid move, stay in place
}

int main() {
    srand(time(NULL));
    init_rooms();

    mosquitto_lib_init();
    mosq = mosquitto_new(NULL, true, NULL);
    mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        return 1;
    }

    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);
    char buffer[BUF_SIZE];

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        return 1;
    }

    printf("Dungeon daemon started. Listening for moves...\n");

    while (1) {
        int n = recvfrom(sockfd, buffer, BUF_SIZE - 1, 0, (struct sockaddr *)&cliaddr, &len);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }

        buffer[n] = '\0';
        printf("Received raw UDP: %s\n", buffer); // DEBUG

        char player[16], cmd[4];
        if (sscanf(buffer, "{\"player\":\"%15[^\"]\",\"cmd\":\"%3[^\"]\"}", player, cmd) == 2) {
            printf("Parsed move: player=%s, cmd=%s\n", player, cmd); // DEBUG
            int idx = find_or_add_player(player);
            int new_room = move_player(players[idx].current_room, cmd);
            players[idx].current_room = new_room;
            send_room_description(player, new_room);
        } else {
            fprintf(stderr, "Invalid command format: %s\n", buffer);
        }
    }

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    close(sockfd);
    return 0;
}