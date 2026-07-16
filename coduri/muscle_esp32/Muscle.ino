#include <WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "HAND_HOST";
const char* password = "12345678";

WebSocketsClient webSocket;

// EMG pins
#define HAND_PIN 34
#define WRIST_PIN 35

float handFiltered = 0;
float wristFiltered = 0;

float handBaseline = 0;
float wristBaseline = 0;

// Correct settings
int handThreshold = 350;
int wristThreshold = 120;

int handResetThreshold = 120;
int wristResetThreshold = 60;

bool handLocked = false;
bool wristLocked = false;

bool wsConnected = false;

unsigned long lastPrint = 0;

void sendCommand(int command) {
  if (!wsConnected) {
    Serial.println("WebSocket not connected, command not sent.");
    return;
  }

  String msg = "M," + String(command);
  webSocket.sendTXT(msg);

  Serial.print("SENT COMMAND: ");
  Serial.println(msg);
}

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

void calibrateBaseline() {
  Serial.println();
  Serial.println("Calibrating baseline...");
  Serial.println("Keep both muscles relaxed.");

  long handSum = 0;
  long wristSum = 0;

  for (int i = 0; i < 200; i++) {
    handSum += analogRead(HAND_PIN);
    wristSum += analogRead(WRIST_PIN);
    delay(3);
  }

  handBaseline = handSum / 200.0;
  wristBaseline = wristSum / 200.0;

  handFiltered = handBaseline;
  wristFiltered = wristBaseline;

  handLocked = false;
  wristLocked = false;

  Serial.print("HAND baseline = ");
  Serial.println(handBaseline);

  Serial.print("WRIST baseline = ");
  Serial.println(wristBaseline);

  Serial.println("Calibration done.");
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  delay(1000);

  Serial.println("MUSCLE CLIENT STARTING");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Connecting to HAND_HOST...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("Muscle IP: ");
  Serial.println(WiFi.localIP());

  webSocket.begin("192.168.4.1", 81, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(2000);

  calibrateBaseline();

  Serial.println("Commands:");
  Serial.println("c = recalibrate baseline");
}

void loop() {
  webSocket.loop();

  int handRaw = analogRead(HAND_PIN);
  int wristRaw = analogRead(WRIST_PIN);

  handFiltered = handFiltered * 0.7 + handRaw * 0.3;
  wristFiltered = wristFiltered * 0.7 + wristRaw * 0.3;

  float handDiff = handFiltered - handBaseline;
  float wristDiff = wristFiltered - wristBaseline;

  float handAbs = abs(handDiff);
  float wristAbs = abs(wristDiff);

  if (!handLocked && handAbs < handThreshold) {
    handBaseline = handBaseline * 0.995 + handFiltered * 0.005;
  }

  if (!wristLocked && wristAbs < wristThreshold && handAbs < handThreshold) {
    wristBaseline = wristBaseline * 0.995 + wristFiltered * 0.005;
  }

  handDiff = handFiltered - handBaseline;
  wristDiff = wristFiltered - wristBaseline;

  handAbs = abs(handDiff);
  wristAbs = abs(wristDiff);

  // HAND priority
  if (!handLocked && handAbs > handThreshold) {
    handLocked = true;
    wristLocked = true;

    Serial.println(">>> HAND FLEX DETECTED <<<");
    sendCommand(1);
  }

  // WRIST only if hand is below threshold
  else if (!wristLocked && wristAbs > wristThreshold && handAbs < handThreshold) {
    wristLocked = true;

    Serial.println(">>> WRIST FLEX DETECTED <<<");
    sendCommand(2);
  }

  if (handLocked && handAbs < handResetThreshold) {
    handLocked = false;
    Serial.println("HAND ready again");
  }

  if (wristLocked && wristAbs < wristResetThreshold && handAbs < handThreshold) {
    wristLocked = false;
    Serial.println("WRIST ready again");
  }

  if (millis() - lastPrint > 500) {
    lastPrint = millis();

    Serial.print("HAND abs: ");
    Serial.print(handAbs);

    Serial.print(" || WRIST abs: ");
    Serial.println(wristAbs);
  }

  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == 'c') {
      calibrateBaseline();
    }
  }

  delay(500);
}