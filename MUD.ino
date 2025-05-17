#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//===== WiFi and Server Settings =====
const char* ssid = "";
const char* password = "";
const char* serverIP   = "35.194.16.121";
const uint16_t serverPort = 12345;

const char* mqttBroker = "35.194.16.121";
const uint16_t mqttPort  = 1883;
const char* mqttTopic    = "dungeon/room";

//===== Joystick Pins =====
const int xPin = 14;
const int yPin = 13;
const int zPin = 12;

//===== Thresholds =====
const int northThreshold = 200;
const int southThreshold = 800;
const int westThreshold  = 200;
const int eastThreshold  = 800;
const int deadMin = 350;
const int deadMax = 650;

//===== LCD Setup =====
LiquidCrystal_I2C lcd(0x27, 16, 2);

//===== Network Clients =====
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
WiFiClient socketClient;

//===== Scroll State =====
String currentMessage = "";
unsigned long lastScrollTime = 0;
int scrollIndex = 0;
bool scrollingForward = true;
const int scrollDelay = 300;
const int restDelay = 1000;
unsigned long scrollPauseUntil = 0;

//===== MQTT Callback =====
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  currentMessage = "";
  for (unsigned int i = 0; i < length; i++) {
    currentMessage += (char)payload[i];
  }

  Serial.print("MQTT Received: ");
  Serial.println(currentMessage);

  scrollIndex = 0;
  scrollingForward = true;
  scrollPauseUntil = millis() + restDelay;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Room:");
}

//===== MQTT Reconnect =====
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
    socketClient.write("S");
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Use Joystick");
}

//===== Scroll Update =====
void updateScrollingText() {
  unsigned long now = millis();
  int width = 16;
  int len = currentMessage.length();

  if (len <= width) {
    lcd.setCursor(0, 1);
    lcd.print(currentMessage);
    return;
  }

  if (now < scrollPauseUntil) return;

  if (now - lastScrollTime >= scrollDelay) {
    lcd.setCursor(0, 1);
    lcd.print(currentMessage.substring(scrollIndex, scrollIndex + width));

    if (scrollingForward) {
      scrollIndex++;
      if (scrollIndex > len - width) {
        scrollIndex = len - width;
        scrollingForward = false;
        scrollPauseUntil = now + restDelay;
      }
    } else {
      scrollIndex--;
      if (scrollIndex < 0) {
        scrollIndex = 0;
        scrollingForward = true;
        scrollPauseUntil = now + restDelay;
      }
    }

    lastScrollTime = now;
  }
}

//===== Main Loop =====
void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  updateScrollingText();

  int xVal = analogRead(xPin);
  int yVal = analogRead(yPin);
  int zVal = digitalRead(zPin);

  static bool buttonPressed = false;
  if (zVal == LOW && !buttonPressed) {
    Serial.println("Joystick Button Pressed");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Button Pressed");
    buttonPressed = true;
  } else if (zVal == HIGH) {
    buttonPressed = false;
  }

  char command = '\0';
  bool inDeadZone = (xVal >= deadMin && xVal <= deadMax && yVal >= deadMin && yVal <= deadMax);

  if (!inDeadZone) {
    if (yVal < northThreshold) command = 'n';
    else if (yVal > southThreshold) command = 's';
    else if (xVal < westThreshold)  command = 'w';
    else if (xVal > eastThreshold)  command = 'e';
  }

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

    if (!socketClient.connected()) {
      socketClient.stop();
      delay(100);
      socketClient.connect(serverIP, serverPort);
    }

    socketClient.write(command);

    // Reset scroll trigger after move
    scrollIndex = 0;
    scrollingForward = true;
    scrollPauseUntil = millis() + restDelay;
  }

  delay(50);
}
