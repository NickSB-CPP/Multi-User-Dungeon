#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int speed_Adjust = 400; // speed of text scroll
int rest_Time = 2000;    // resting time of text animation at edge of display

//===== WiFi Credentials =====
const char* ssid = "iPhone (3)";
const char* password = "7ehs17k64dx6j";

//===== Server (Game Logic) Settings =====
const char* serverIP = "35.188.222.48";
const uint16_t serverPort = 12345;

//===== MQTT Broker Settings =====
const char* mqttBroker = "35.188.222.48";
const uint16_t mqttPort = 1883;
const char* mqttTopic  = "dungeon/room";

//===== Joystick Pins =====
const int xPin = 14;
const int yPin = 13;
const int zPin = 12;

//===== Joystick Thresholds =====
const int northThreshold = 200;
const int southThreshold = 800;
const int westThreshold  = 200;
const int eastThreshold  = 800;

//===== Dead Zone =====
const int deadMin = 350;
const int deadMax = 650;

//===== I2C LCD Setup =====
LiquidCrystal_I2C lcd(0x27, 16, 2);

//===== Clients =====
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
WiFiClient socketClient;

//===== Scroll Function =====
void scrollText(String text, int line = 1) {
  int len = text.length();
  int width = 16;

  if (len <= width) {
    lcd.setCursor(0, line);
    lcd.print(text);
    delay(rest_Time);
    return;
  }

  // Scroll right
  for (int i = 0; i <= len - width; i++) {
    lcd.setCursor(0, line);
    lcd.print(text.substring(i, i + width));
    delay(speed_Adjust);
  }

  delay(rest_Time); // pause at end

  // Scroll left
  for (int i = len - width - 1; i >= 0; i--) {
    lcd.setCursor(0, line);
    lcd.print(text.substring(i, i + width));
    delay(speed_Adjust);
  }

  delay(rest_Time); // pause at start
}

//===== MQTT Callback =====
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message(reinterpret_cast<char*>(payload), length);

  Serial.print("Room: ");
  Serial.println(message);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Room:");
  scrollText(message, 1);
}

//===== Reconnect MQTT =====
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32S3_Controller")) {
      mqttClient.subscribe(mqttTopic);
    } else {
      delay(2000);
    }
  }
}

//===== Setup =====
void setup() {
  Serial.begin(115200);
  analogReadResolution(10);

  pinMode(zPin, INPUT_PULLUP); // Initialize joystick button

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
  int zVal = digitalRead(zPin);

  Serial.print("X: ");
  Serial.print(xVal);
  Serial.print(" | Y: ");
  Serial.print(yVal);
  Serial.print(" | Z: ");
  Serial.println(zVal);

  static bool joystickMoved = false;
  char command = '\0';

  bool inDeadZone = (xVal >= deadMin && xVal <= deadMax && yVal >= deadMin && yVal <= deadMax);

  if (yVal < northThreshold) {
    command = 'n';
  } else if (yVal > southThreshold) {
    command = 's';
  } else if (xVal < westThreshold) {
    command = 'w';
  } else if (xVal > eastThreshold) {
    command = 'e';
  }

  if (command != '\0' && !joystickMoved && !inDeadZone) {
    String direction = "";
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

    delay(2000);

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

  if (inDeadZone) {
    joystickMoved = false;
  }

  delay(100);
}
