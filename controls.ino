#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//===== WiFi Credentials =====
const char* ssid = "wifi";
const char* password = "wifi password";

//===== Game Server (TCP) Settings =====
const char* serverIP = "GCP IP"; // e.g., "34.123.45.67"
const uint16_t serverPort = 12345;

//===== MQTT Broker Settings =====
const char* mqttBroker = "GCP IP"; // Same as serverIP if running the broker there
const uint16_t mqttPort = 1883;     
const char* mqttTopic = "dungeon/room";

//===== Joystick Pins =====
const int xPin = 14; // Analog input for X
const int yPin = 13; // Analog input for Y
const int zPin = 12; // Digital input for button Z

//===== Joystick Extreme Thresholds =====
const int northThreshold = 200; 
const int southThreshold = 800;
const int westThreshold = 200;  
const int eastThreshold = 800;  

//===== Dead Zone Boundaries =====
const int deadMin = 350; // Joystick considered idle if X and Y in [350,650]
const int deadMax = 650;

//===== I2C LCD Setup =====
LiquidCrystal_I2C lcd(0x27, 16, 2);

//===== Clients =====
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
WiFiClient socketClient;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Room: ");
  Serial.println(message);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Room:");
  lcd.setCursor(0, 1);
  lcd.print(message.substring(0, 16));
}

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
  analogReadResolution(10);
  pinMode(zPin, INPUT_PULLUP);
  Wire.begin(10, 9);
  
  lcd.init();
  lcd.backlight();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  mqttClient.setServer(mqttBroker, mqttPort);
  mqttClient.setCallback(mqttCallback);
  
  if (socketClient.connect(serverIP, serverPort)) {
    // Optionally send a start signal once
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
  
  int xVal = analogRead(xPin);
  int yVal = analogRead(yPin);
  int zVal = digitalRead(zPin); // LOW when pressed.
  
  // Debug output.
  Serial.print("X: ");
  Serial.print(xVal);
  Serial.print(" | Y: ");
  Serial.print(yVal);
  Serial.print(" | Z: ");
  Serial.println(zVal);
  
  // Check for button press (if needed)
  static bool buttonPressed = false;
  if (zVal == LOW) {
    if (!buttonPressed) {
      Serial.println("Joystick button pressed!");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Button Pressed");
      delay(300);  // A short debounce delay.
      buttonPressed = true;
    }
  } else {
    buttonPressed = false;
  }
  
  // Compute directional command based solely on the analog readings.
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
  
  // If a valid command is detected, then send it immediately.
  if (command != '\0') {
    String direction;
    switch (command) {
      case 'n': direction = "North"; break;
      case 's': direction = "South"; break;
      case 'w': direction = "West";  break;
      case 'e': direction = "East";  break;
    }
    Serial.print("Move: ");
    Serial.println(direction);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Move:");
    lcd.setCursor(0, 1);
    lcd.print(direction);
    delay(50); // A very short delay to debounce.
    
    if (!socketClient.connected()) {
      socketClient.stop();
      delay(100);
      socketClient.connect(serverIP, serverPort);
    }
    socketClient.write(command);
  }
  
  delay(50); // A very short loop delay to allow fast repeated commands.
}
