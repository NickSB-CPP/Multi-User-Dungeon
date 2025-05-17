#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <mosquitto.h>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
// getch implementation for POSIX systems
int getch(void) {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt); // save old terminal settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // disable buffered input and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // apply new settings
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restore old settings
    return ch;
}
#endif

#define MQTT_BROKER "35.226.70.79" // Update to your MQTT broker IP
#define MQTT_PORT 1883
#define MQTT_TOPIC "dungeon/start"

int main() {
    struct mosquitto *mosq = NULL;
    int ret;
    
    /* Initialize the mosquitto library */
    mosquitto_lib_init();
    
    /* Create a new mosquitto client instance */
    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        fprintf(stderr, "Error: Could not create mosquitto instance.\n");
        return 1;
    }
    
    /* Connect to the MQTT broker */
    ret = mosquitto_connect(mosq, MQTT_BROKER, MQTT_PORT, 60);
    if (ret != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: Could not connect to MQTT Broker. Error Code: %d\n", ret);
        mosquitto_destroy(mosq);
        return 1;
    }
    
    /* Display the start menu on the console */
    printf("=====================================\n");
    printf("  WELCOME TO THE DUNGEON\n");
    printf("=====================================\n\n");
    printf("You are about to embark on a mysterious journey\n");
    printf("through a labyrinth filled with hidden treasure.\n");
    printf("The maze is tricky, and only the brave will succeed.\n\n");
    printf("To begin your adventure, press the [Z] key.\n");
    printf("This corresponds to pressing your joystick button on the ESP32 console.\n");
    printf("=====================================\n");
    
    /* Wait for the user to press 'z' or 'Z' */
    char ch = 0;
    while (1) {
        ch = getch();
        if (ch == 'z' || ch == 'Z')
            break;
    }
    printf("\nStarting game...\n");
    
    /* Publish the start signal to the MQTT topic */
    const char *startMsg = "start";
    ret = mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(startMsg), startMsg, 0, false);
    if (ret != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: Could not publish the start message. Error Code: %d\n", ret);
    } else {
        printf("Start signal sent to ESP32.\n");
    }
    
    /* Clean up and disconnect */
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    
    return 0;
}
