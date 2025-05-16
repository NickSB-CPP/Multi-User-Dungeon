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
    This is my map with a theme of being in a forest while you can find the treasure there
    (that is my item.)
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//===== WiFi Credentials =====
const char* ssid = "wifi";
const char* password = "wifi password";

//===== Server (Game Logic) Settings =====
const char* serverIP = "GCP IP"; // e.g., "34.123.45.67"
const uint16_t serverPort = 12345;

//===== MQTT Broker Settings =====
const char* mqttBroker = "GCP IP"; // Same as serverIP if running the broker there
const uint16_t mqttPort = 1883;                   // Default MQTT port
const char* mqttTopic  = "dungeon/room";

//===== Joystick Pins =====
const int xPin = 14;  // Analog input for X
const int yPin = 13;  // Analog input for Y
const int zPin = 12;  // Digital input for button Z (typically pulled HIGH when not pressed)

//===== Joystick Extreme Thresholds =====
const int northThreshold = 200;  // Move North if Y < 200
const int southThreshold = 800;  // Move South if Y > 800
const int westThreshold  = 200;  // Move West if X < 200
const int eastThreshold  = 800;  // Move East if X > 800

//===== Dead Zone Boundaries =====
const int deadMin = 350; // If X and Y are between 350 and 650, joystick is idle
const int deadMax = 650;

//===== I2C LCD Setup =====
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adjust I2C address if needed

//===== Clients =====
WiFiClient wifiClient;       // For MQTT connection
PubSubClient mqttClient(wifiClient);
WiFiClient socketClient;     // For TCP connection to the game server

// Update LCD with room description
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  // Print room description to the Serial Monitor.
  Serial.print("Room: ");
  Serial.println(message);
  
  // Update LCD with the room description.
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Room:");
  lcd.setCursor(0, 1);
  lcd.print(message.substring(0, 16)); // Show only up to 16 characters per line.
}

//----- Reconnect to MQTT if disconnected -----
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32S3_Controller")) {
      mqttClient.subscribe(mqttTopic);
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Set ADC resolution to 10 bits
  analogReadResolution(10);
  
  // Initialize I2C for LCD (SDA on pin 10, SCL on pin 9).
  Wire.begin(10, 9);
  lcd.init();
  lcd.backlight();
  
  // Connect to WiFi.
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  // Setup MQTT.
  mqttClient.setServer(mqttBroker, mqttPort);
  mqttClient.setCallback(mqttCallback);
  
  // Connect to the game server via TCP.
  if (socketClient.connect(serverIP, serverPort)) {
    socketClient.write("S"); // Optional start command.
  }
  
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Use Joystick");
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
  
  // Read analog values and digital Z.
  int xVal = analogRead(xPin);
  int yVal = analogRead(yPin);
  int zVal = digitalRead(zPin);  // This value typically remains HIGH until pressed
  
  // debugging for movements.
  Serial.print("X: ");
  Serial.print(xVal);
  Serial.print(" | Y: ");
  Serial.print(yVal);
  Serial.print(" | Z: ");
  Serial.println(zVal);
  
  // Use a static flag to register only one move until the joystick returns to idle.
  static bool joystickMoved = false;
  char command = '\0';
  
  // Checks if joystick is idle
  bool inDeadZone = (xVal >= deadMin && xVal <= deadMax && yVal >= deadMin && yVal <= deadMax);
  
  // Determine the command based on extreme values.
  if (yVal < northThreshold) {
    command = 'n';
  } else if (yVal > southThreshold) {
    command = 's';
  } else if (xVal < westThreshold) {
    command = 'w';
  } else if (xVal > eastThreshold) {
    command = 'e';
  }
  
  // If valid command, the joystick hasn’t already triggered a move,
  // and the joystick is not in the dead zone, then register a move.
  if (command != '\0' && !joystickMoved && !inDeadZone) {
    String direction = "";
    switch (command) {
      case 'n': direction = "North"; break;
      case 's': direction = "South"; break;
      case 'w': direction = "West";  break;
      case 'e': direction = "East";  break;
    }
    
    // Print the move direction to the Serial Monitor.
    Serial.print("Move: ");
    Serial.println(direction);
    
    // Display the move on the LCD.
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Move:");
    lcd.setCursor(0, 1);
    lcd.print(direction);
    
    // Hold this display for 2 seconds.
    delay(2000);
    
    // Send the movement command via TCP.
    if (socketClient.connected()) {
      socketClient.write(command);
    } else {
      socketClient.stop();
      delay(1000);
      if (socketClient.connect(serverIP, serverPort)) {
        socketClient.write("S");
        socketClient.write(command);
      }
    }
    
    joystickMoved = true;  
    delay(200);            
  }
  
  // Reset the move flag once the joystick returns to the dead zone.
  if (inDeadZone) {
    joystickMoved = false;
  }
  
  delay(100);
}
