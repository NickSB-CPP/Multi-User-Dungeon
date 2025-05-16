#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//===== WiFi Credentials =====
const char* ssid = "wifi";
const char* password = "wifi password";

//===== Server (Game Logic) Settings =====
const char* serverIP = "ip"; // e.g., "34.123.45.67"
const uint16_t serverPort = 12345;

//===== MQTT Broker Settings =====
const char* mqttBroker = "enter ip"; // Same as serverIP if broker runs there
const uint16_t mqttPort = 1883;                   // Default MQTT port
const char* mqttTopic  = "dungeon/room";

//===== Joystick Pins =====
const int xPin = 14;  // Analog input for X-axis
const int yPin = 13;  // Analog input for Y-axis
const int zPin = 12;  // Digital input for the button (Z) -- typically pulled HIGH when not pressed

//===== Joystick Extreme Thresholds =====
// Only register a move if the reading goes below or above these values.
const int northThreshold = 200;  // For Y-axis: move North if value < 200.
const int southThreshold = 800;  // For Y-axis: move South if value > 800.
const int westThreshold  = 200;  // For X-axis: move West if value < 200.
const int eastThreshold  = 800;  // For X-axis: move East if value > 800.

//===== Dead Zone Boundaries =====
// If the joystick readings (for both X and Y) are within these values, consider it idle.
const int deadMin = 350;
const int deadMax = 650;

//===== I2C LCD Setup =====
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adjust I²C address if required

//===== Clients =====
WiFiClient wifiClient;       // For MQTT connection
PubSubClient mqttClient(wifiClient);
WiFiClient socketClient;     // For the TCP connection to the game server

//----- MQTT Callback -----
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  // Print the room description on the Serial Monitor.
  Serial.print("Room: ");
  Serial.println(message);
  
  // Update the LCD with the room description.
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Room Update:");
  lcd.setCursor(0, 1);
  lcd.print(message.substring(0, 16));
}

//----- Reconnect to MQTT -----
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
    // Optional: send a start command.
    socketClient.write("S");
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
  
  // Read analog values (for X and Y) and the digital value for Z.
  int xVal = analogRead(xPin);
  int yVal = analogRead(yPin);
  int zVal = digitalRead(zPin);  // Typically remains HIGH until pressed
  
  // debugging for movements.
  Serial.print("X: "); Serial.print(xVal);
  Serial.print(" | Y: "); Serial.print(yVal);
  Serial.print(" | Z: "); Serial.println(zVal);
  
  // Use a static flag so that a single move is registered until the joystick returns to idle.
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
    
    // Print the move direction on the Serial Monitor.
    Serial.print("Move: ");
    Serial.println(direction);
    
    // Display the move direction on the LCD.
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Move:");
    lcd.setCursor(0, 1);
    lcd.print(direction);
    
    // Hold the move display for 2 seconds.
    delay(2000);
    
    // Send the movement command to the game server.
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
  
  // Reset the joystickMoved flag when the joystick returns to idle.
  if (inDeadZone) {
    joystickMoved = false;
  }
  
  delay(100);
}
