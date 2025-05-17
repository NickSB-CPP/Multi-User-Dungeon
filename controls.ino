
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//===== WiFi and Server Settings =====
const char* ssid       = "wifi";
const char* password   = "wifi password";
const char* serverIP   = "GCP IP";        // Replace with your server's IP address.
const uint16_t serverPort = 12345;         // Server TCP port.

const char* mqttBroker = "GCP IP";         // Replace with your MQTT broker IP.
const uint16_t mqttPort  = 1883;
const char* mqttTopic    = "dungeon/room";

//===== Joystick Pins =====
const int xPin = 14;  // Analog x-axis
const int yPin = 13;  // Analog y-axis
const int zPin = 12;  // Digital button; using internal pull-up

//===== Joystick Thresholds =====
const int northThreshold = 200;  
const int southThreshold = 800;
const int westThreshold  = 200;
const int eastThreshold  = 800;
const int deadMin = 350;  // Dead zone minimum
const int deadMax = 650;  // Dead zone maximum

//===== LCD Setup =====
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Change I²C address if needed

//===== Network Clients =====
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
WiFiClient socketClient;

//===== MQTT Callback =====
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("MQTT Received: ");
  Serial.println(message);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Room:");
  lcd.setCursor(0, 1);
  lcd.print(message.substring(0, 16));  // Show first 16 characters
}

//===== Reconnect to MQTT Broker =====
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32_Controller")) {
      mqttClient.subscribe(mqttTopic);
    } else {
      delay(2000);
    }
  }
}

//===== Setup =====
void setup() {
  Serial.begin(115200);
  analogReadResolution(10); // 10-bit ADC (0-1023)
  
  // Configure button pin with internal pull-up.
  pinMode(zPin, INPUT_PULLUP);
  
  // Initialize I2C and LCD.
  Wire.begin(10, 9); // Adjust SDA/SCL pins as needed.
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
    // Optionally send a start signal once (e.g., "S").
    socketClient.write("S");
  }
  
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Use Joystick");
}

//===== Main Loop =====
void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
  
  int xVal = analogRead(xPin);
  int yVal = analogRead(yPin);
  int zVal = digitalRead(zPin);  // Because of INPUT_PULLUP, LOW means pressed.
  
  // Print raw readings for debugging.
  Serial.print("X: ");
  Serial.print(xVal);
  Serial.print(" | Y: ");
  Serial.print(yVal);
  Serial.print(" | Z: ");
  Serial.println(zVal);
  
  // Handle button press (for instance, for starting game or special action).
  static bool buttonPressed = false;
  if (zVal == LOW) {
    if (!buttonPressed) {
      Serial.println("Joystick Button Pressed");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Button Pressed");
      delay(200);  // Simple debounce delay.
      buttonPressed = true;
    }
  } else {
    buttonPressed = false;
  }
  
  // Determine directional command.
  char command = '\0';
  bool inDeadZone = (xVal >= deadMin && xVal <= deadMax && yVal >= deadMin && yVal <= deadMax);
  if (!inDeadZone) {
    if (yVal < northThreshold) {
      command = 'n';
    } else if (yVal > southThreshold) {
      command = 's';
    } else if (xVal < westThreshold) {
      command = 'w';
    } else if (xVal > eastThreshold) {
      command = 'e';
    }
  }
  
  // If a valid directional command is determined, send it.
  if (command != '\0') {
    String direction;
    switch (command) {
      case 'n': direction = "North"; break;
      case 's': direction = "South"; break;
      case 'w': direction = "West";  break;
      case 'e': direction = "East";  break;
    }
    Serial.print("Direction Command: ");
    Serial.println(direction);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Move:");
    lcd.setCursor(0, 1);
    lcd.print(direction);
    delay(50);  // Short delay to allow the move to register.
    
    // Ensure TCP connection is valid; reconnect if needed.
    if (!socketClient.connected()) {
      socketClient.stop();
      delay(100);
      socketClient.connect(serverIP, serverPort);
    }
    socketClient.write(command);  // Send a single-character command ('n', 's', 'e', or 'w').
  }
  
  delay(50);  // A small loop delay.
}
