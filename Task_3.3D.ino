#include <WiFiNINA.h>
#include <PubSubClient.h>

const char* ssid = "A16";            
const char* password = "22222222";   

const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;        

const char* topic_wave = "SIT210/wave";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

#define TRIG_PIN 2
#define ECHO_PIN 3
#define LED_PIN 4

void flashLED(int times, int delayTime = 500);  

void setup() {

  Serial.begin(9600);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  connectWiFi();
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
  
  connectMQTT();
}

void loop() {
 
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();

  long distance = measureDistance();

  if (distance < 20) {
    Serial.println("Wave detected!");
    client.publish(topic_wave, "Yashpinder");
    delay(1000); 
  }
}

void connectWiFi() {
 
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("Connected to Wi-Fi");
}

void connectMQTT() {
  
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    
    if (client.connect("ArduinoClient")) {  
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
 
  Serial.print("Message received: ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  flashLED(3);
}

long measureDistance() {
 
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
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(delayTime);
    digitalWrite(LED_PIN, LOW);
    delay(delayTime);
  }
}
