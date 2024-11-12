#include <WiFiNINA.h>       // Library to handle Wi-Fi connection
#include <PubSubClient.h>   // Library for MQTT client functionality

// Wi-Fi credentials
const char* ssid = "A16";             // Wi-Fi network name (SSID)
const char* password = "22222222";    // Wi-Fi network password

// MQTT broker settings
const char* mqtt_server = "broker.emqx.io";  // MQTT broker URL
const int mqtt_port = 1883;                  // MQTT port for non-secure connections

// MQTT topic for publishing wave detection events
const char* topic_wave = "SIT210/wave";

// Wi-Fi and MQTT client setup
WiFiClient wifiClient;         // Wi-Fi client instance
PubSubClient client(wifiClient); // MQTT client using Wi-Fi client

// Pin assignments
#define TRIG_PIN 2     // Trigger pin for ultrasonic sensor
#define ECHO_PIN 3     // Echo pin for ultrasonic sensor
#define LED_PIN 4      // LED pin for visual feedback

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize pins for the ultrasonic sensor and LED
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Connect to Wi-Fi
  connectWiFi();
  
  // Set MQTT server and callback function
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
  
  // Connect to the MQTT broker
  connectMQTT();
}

void loop() {
  // Check MQTT connection and reconnect if necessary
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop(); // Keep MQTT client processing

  // Measure distance using the ultrasonic sensor
  long distance = measureDistance();

  // Publish a message if an object is detected within 20 cm
  if (distance < 20) {
    Serial.println("Wave detected!");
    client.publish(topic_wave, "Yashpinder");  // Publish message to topic
    delay(1000);  // Delay to prevent multiple messages for the same detection
  }
}

// Function to connect to Wi-Fi
void connectWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);  // Start Wi-Fi connection
  
  // Wait until Wi-Fi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("Connected to Wi-Fi");
}

// Function to connect to MQTT broker
void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    
    // Attempt to connect with client ID "ArduinoClient"
    if (client.connect("ArduinoClient")) {
      Serial.println("Connected");
      client.subscribe(topic_wave);  // Subscribe to wave detection topic
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);  // Wait and try again if connection failed
    }
  }
}

// Callback function to handle received MQTT messages
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  
  // Print the received payload
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  flashLED(3);  // Flash LED 3 times for notification
}

// Function to measure distance using the ultrasonic sensor
long measureDistance() {
  // Trigger the ultrasonic sensor to start measurement
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Measure the time of the pulse in microseconds
  long duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calculate distance based on the duration (in centimeters)
  long distance = duration * 0.034 / 2;
  
  return distance;
}

// Function to flash the LED a specific number of times with a delay
void flashLED(int times, int delayTime = 500) {  // Default delay is 500ms if not specified
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);  // Turn LED on
    delay(delayTime);             // Wait
    digitalWrite(LED_PIN, LOW);   // Turn LED off
    delay(delayTime);             // Wait
  }
}
