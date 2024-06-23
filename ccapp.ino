#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>

#define timA 4
#define timB 2
#define timC 15

#define pilgan1 32
#define pilgan2 33
#define pilgan3 27
#define pilgan4 14

const char* ssid = "CC By Mabduh-Hanif-Wartono";
const char* password = "11110000";

String message = "{\"tim\":0}";
String pilganMessage = "{\"pilgan\":0}";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); 

int buzzerPin = 21;  // GPIO untuk buzzer
const int ledcChannel = 0;
const int ledcResolution = 8; // 8-bit resolution

void setup() {
  pinMode(buzzerPin, OUTPUT); 
  pinMode(timA, INPUT_PULLUP);
  pinMode(timB, INPUT_PULLUP);
  pinMode(timC, INPUT_PULLUP);
  pinMode(pilgan1, INPUT_PULLUP);
  pinMode(pilgan2, INPUT_PULLUP);
  pinMode(pilgan3, INPUT_PULLUP);
  pinMode(pilgan4, INPUT_PULLUP);
  Serial.begin(115200); 

  WiFi.softAP(ssid, password);
  Serial.println("Access Point Created");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Initialize LEDC to control buzzer
  ledcSetup(ledcChannel, 1000, ledcResolution);
  ledcAttachPin(buzzerPin, ledcChannel);

  ws.onEvent(onWsEvent);
  server.addHandler(&ws); 
  server.begin();
}

void loop() {
  int stateA = digitalRead(timA);
  int stateB = digitalRead(timB);
  int stateC = digitalRead(timC);

  int pil1 = digitalRead(pilgan1);
  int pil2 = digitalRead(pilgan2);
  int pil3 = digitalRead(pilgan3);
  int pil4 = digitalRead(pilgan4);
  
  if (message == "{\"tim\":0}") {
    if (stateA == LOW) {
      Serial.println("TIM A button pressed!");
      message = "{\"tim\":1}";
      ws.textAll(message); 
      buzzerSound(1, 150, 1000);
    }
    
    if (stateB == LOW) {
      Serial.println("TIM B button pressed!");
      message = "{\"tim\":2}";
      ws.textAll(message); 
      buzzerSound(2, 100, 1000);
    }
    
    if (stateC == LOW) {
      Serial.println("TIM C button pressed!");
      message = "{\"tim\":3}";
      ws.textAll(message); 
      buzzerSound(3, 50, 1000);  
    }
  } else {
    pilgan(pil1, pil2, pil3, pil4);
  }
}

void pilgan(int pil1, int pil2, int pil3, int pil4) {
  if (pilganMessage == "{\"pilgan\":0}") {
    if (pil1 == LOW) {
      Serial.println("pilgan 1 button pressed!");
      pilganMessage = "{\"pilgan\":1}";
      ws.textAll(pilganMessage);
      buzzerSound(5, 30, 1000);
    }
    if (pil2 == LOW) {
      Serial.println("pilgan 2 button pressed!");
      pilganMessage = "{\"pilgan\":2}";
      ws.textAll(pilganMessage); 
      buzzerSound(5, 30, 1000);
    }
    if (pil3 == LOW) {
      Serial.println("pilgan 3 button pressed!");
      pilganMessage = "{\"pilgan\":3}";
      ws.textAll(pilganMessage); 
      buzzerSound(5, 30, 1000);
    }
    if (pil4 == LOW) {
      Serial.println("pilgan 4 button pressed!");
      pilganMessage = "{\"pilgan\":4}";
      ws.textAll(pilganMessage);
      buzzerSound(5, 30, 1000);
    }
  }
}

void buzzerSound(int count, int longdelay, int freq) {
  for (int i = 0; i < count; i++) {
    ledcWriteTone(ledcChannel, freq);
    delay(longdelay);
    ledcWriteTone(ledcChannel, 0);
    delay(longdelay); 
  }
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("Client connected: %u\n", client->id());
    message = "{\"tim\":0}";
    pilganMessage = "{\"pilgan\":0}";
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("Client disconnected: %u\n", client->id());
    message = "{\"tim\":0}";
    pilganMessage = "{\"pilgan\":0}";
  } else if (type == WS_EVT_DATA) {
    String msg = String((char*)data).substring(0, len);
    Serial.printf("Data received from client %u: %s\n", client->id(), msg.c_str());

    if (msg == "reset") {
      message = "{\"tim\":0}";
      pilganMessage = "{\"pilgan\":0}";
      ws.textAll("hasil direset");
    }
  }
}
