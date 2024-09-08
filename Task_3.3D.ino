#include <WiFiNINA.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "A16";            // Your WiFi SSID
const char* password = "22222222";   // Your WiFi password

// MQTT broker details
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;          // TCP Port for MQTT

// MQTT topics
const char* topic_wave = "SIT210/wave";

// WiFi and MQTT clients
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Pins for ultrasonic sensor
#define TRIG_PIN 2
#define ECHO_PIN 3
#define LED_PIN 4

void flashLED(int times, int delayTime = 500);  // Function prototype

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  
  // Set up pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Connect to Wi-Fi
  connectWiFi();
  
  // Set MQTT server and callback function
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
  
  // Connect to MQTT broker
  connectMQTT();
}

void loop() {
  // Reconnect if disconnected
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();

  // Measure distance
  long distance = measureDistance();

  // Detect wave (adjust threshold as needed)
  if (distance < 20) {
    Serial.println("Wave detected!");
    client.publish(topic_wave, "Yashpinder");
    delay(1000);  // Debounce delay
  }
}

void connectWiFi() {
  // Connect to Wi-Fi network
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("Connected to Wi-Fi");
}

void connectMQTT() {
  // Connect to MQTT broker
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    
    if (client.connect("ArduinoClient")) {  // Client ID
      Serial.println("Connected");
      client.subscribe(topic_wave);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming messages
  Serial.print("Message received: ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Flash LED 3 times on receiving message
  flashLED(3);
}

long measureDistance() {
  // Measure distance with ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2;
  
  return distance;
}

void flashLED(int times, int delayTime) {
  // Flash LED specified number of times
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(delayTime);
    digitalWrite(LED_PIN, LOW);
    delay(delayTime);
  }
}
