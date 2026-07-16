#include <WiFi.h>
#include <WebSocketsClient.h>

#define NUM_FINGERS 5

const char* ssid = "HAND_HOST";
const char* password = "12345678";

WebSocketsClient webSocket;

const char* fingerName[NUM_FINGERS] = {
  "Thumb",
  "Index",
  "Middle",
  "Ring",
  "Pinky"
};

// HW-040 pins
int clkPin[NUM_FINGERS] = {
  13,  // Thumb
  27,  // Index
  18,  // Middle
  21,  // Ring
  25   // Pinky
};

int dtPin[NUM_FINGERS] = {
  14,  // Thumb
  33,  // Index
  19,  // Middle
  22,  // Ring
  26   // Pinky
};

// Calibrated directions
int direction[NUM_FINGERS] = {
  1,   // Thumb
 -1,   // Index
 -1,   // Middle
  1,   // Ring
  1    // Pinky
};

// Calibrated max steps
int maxSteps[NUM_FINGERS] = {
  8,  // Thumb
  4,  // Index
  9,  // Middle
  5,  // Ring
  4   // Pinky
};

// Servo calibrated values
int servoOpen[NUM_FINGERS] = {
  20,   // Thumb
  8,    // Index
  178,  // Middle
  8,    // Ring
  80    // Pinky
};

int servoClose[NUM_FINGERS] = {
  124,  // Thumb
  120,  // Index
  48,   // Middle
  180,  // Ring
  110   // Pinky
};

int rawSteps[NUM_FINGERS] = {0, 0, 0, 0, 0};
int lastCLK[NUM_FINGERS];

bool wsConnected = false;

unsigned long lastSend = 0;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_CONNECTED) {
    wsConnected = true;
    Serial.println("WebSocket connected!");
  }

  if (type == WStype_DISCONNECTED) {
    wsConnected = false;
    Serial.println("WebSocket disconnected!");
  }
}

int correctedSteps(int finger) {
  int corrected = rawSteps[finger] * direction[finger];
  corrected = constrain(corrected, 0, maxSteps[finger]);
  return corrected;
}

int mappedServoAngle(int finger) {
  int s = correctedSteps(finger);

  return map(
    s,
    0,
    maxSteps[finger],
    servoOpen[finger],
    servoClose[finger]
  );
}

void resetAll() {
  for (int i = 0; i < NUM_FINGERS; i++) {
    rawSteps[i] = 0;
  }

  Serial.println("All glove counters reset.");
}

void sendAngles() {
  String msg = "G," +
               String(mappedServoAngle(0)) + "," +
               String(mappedServoAngle(1)) + "," +
               String(mappedServoAngle(2)) + "," +
               String(mappedServoAngle(3)) + "," +
               String(mappedServoAngle(4));

  if (wsConnected) {
    webSocket.sendTXT(msg);
  }

  Serial.print("Sent: ");
  Serial.println(msg);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("GLOVE CLIENT STARTING");

  for (int i = 0; i < NUM_FINGERS; i++) {
    pinMode(clkPin[i], INPUT_PULLUP);
    pinMode(dtPin[i], INPUT_PULLUP);
    lastCLK[i] = digitalRead(clkPin[i]);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Connecting to HAND_HOST...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("Glove IP: ");
  Serial.println(WiFi.localIP());

  webSocket.begin("192.168.4.1", 81, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(2000);

  Serial.println("Type r to reset glove counters.");
}

void loop() {
  webSocket.loop();

  for (int i = 0; i < NUM_FINGERS; i++) {
    int currentCLK = digitalRead(clkPin[i]);

    if (currentCLK != lastCLK[i]) {
      if (digitalRead(dtPin[i]) != currentCLK) {
        rawSteps[i]++;
      } else {
        rawSteps[i]--;
      }

      rawSteps[i] = constrain(rawSteps[i], -20, 20);
    }

    lastCLK[i] = currentCLK;
  }

  if (millis() - lastSend > 100) {
    lastSend = millis();
    sendAngles();
  }

  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == 'r') {
      resetAll();
    }
  }
}