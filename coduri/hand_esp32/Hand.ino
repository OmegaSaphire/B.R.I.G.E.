#include <WiFi.h>
#include <WebSocketsServer.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

const char* ssid = "HAND_HOST";
const char* password = "12345678";

WebSocketsServer webSocket(81);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

#define NUM_FINGERS 5

// PCA9685 channels
#define THUMB_CH   0
#define INDEX_CH   1
#define MIDDLE_CH  2
#define RING_CH    3
#define PINKY_CH   4
#define WRIST_CH   5

int servoChannel[NUM_FINGERS] = {
  THUMB_CH,
  INDEX_CH,
  MIDDLE_CH,
  RING_CH,
  PINKY_CH
};

// Adjust if needed for your servos
#define SERVO_MIN 110
#define SERVO_MAX 490

// Finger calibrated values
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

// Wrist limits
int wristRelaxed = 0;
int wristFlexed  = 90;

bool handOverrideClosed = false;
bool wristState = false; // false = 0, true = 90

unsigned long lastGlovePacket = 0;

uint16_t angleToPulse(int angle) {
  angle = constrain(angle, 0, 180);
  return map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
}

void setPCAAngle(uint8_t channel, int angle) {
  pwm.setPWM(channel, 0, angleToPulse(angle));
}

void setFinger(int id, int angle) {
  int minSafe = min(servoOpen[id], servoClose[id]);
  int maxSafe = max(servoOpen[id], servoClose[id]);

  angle = constrain(angle, minSafe, maxSafe);
  setPCAAngle(servoChannel[id], angle);
}

void openHand() {
  Serial.println("HAND -> OPEN");
  for (int i = 0; i < NUM_FINGERS; i++) {
    setFinger(i, servoOpen[i]);
    delay(60);
  }
}

void closeHand() {
  Serial.println("HAND -> CLOSED");
  for (int i = 0; i < NUM_FINGERS; i++) {
    setFinger(i, servoClose[i]);
    delay(60);
  }
}

void setWrist(int angle) {
  angle = constrain(angle, min(wristRelaxed, wristFlexed), max(wristRelaxed, wristFlexed));
  setPCAAngle(WRIST_CH, angle);

  Serial.print("WRIST -> ");
  Serial.println(angle);
}

void handleGlovePacket(String data) {
  // Format: G,thumb,index,middle,ring,pinky

  if (handOverrideClosed) {
    // When EMG hand override is active, glove does not move fingers
    return;
  }

  int c0 = data.indexOf(',');
  int c1 = data.indexOf(',', c0 + 1);
  int c2 = data.indexOf(',', c1 + 1);
  int c3 = data.indexOf(',', c2 + 1);
  int c4 = data.indexOf(',', c3 + 1);

  if (c0 == -1 || c1 == -1 || c2 == -1 || c3 == -1 || c4 == -1) {
    Serial.println("Bad glove packet");
    return;
  }

  int thumb  = data.substring(c0 + 1, c1).toInt();
  int index  = data.substring(c1 + 1, c2).toInt();
  int middle = data.substring(c2 + 1, c3).toInt();
  int ring   = data.substring(c3 + 1, c4).toInt();
  int pinky  = data.substring(c4 + 1).toInt();

  setFinger(0, thumb);
  setFinger(1, index);
  setFinger(2, middle);
  setFinger(3, ring);
  setFinger(4, pinky);

  lastGlovePacket = millis();

  Serial.print("GLOVE -> ");
  Serial.println(data);
}

void handleMusclePacket(String data) {
  // Format: M,1 or M,2

  int comma = data.indexOf(',');
  if (comma == -1) return;

  int command = data.substring(comma + 1).toInt();

  Serial.print("MUSCLE COMMAND -> ");
  Serial.println(command);

  if (command == 1) {
    handOverrideClosed = !handOverrideClosed;

    if (handOverrideClosed) {
      closeHand();
    } else {
      openHand();
      Serial.println("Glove control active again.");
    }
  }

  if (command == 2) {
    wristState = !wristState;

    if (wristState) {
      setWrist(wristFlexed);
    } else {
      setWrist(wristRelaxed);
    }
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_CONNECTED) {
    Serial.print("Client connected: ");
    Serial.println(num);
  }

  if (type == WStype_DISCONNECTED) {
    Serial.print("Client disconnected: ");
    Serial.println(num);
  }

  if (type == WStype_TEXT) {
    String data = String((char*)payload);

    if (data.startsWith("G,")) {
      handleGlovePacket(data);
    } 
    else if (data.startsWith("M,")) {
      handleMusclePacket(data);
    } 
    else {
      Serial.print("Unknown packet: ");
      Serial.println(data);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("HAND HOST STARTING");

  Wire.begin(21, 22);

  pwm.begin();
  pwm.setPWMFreq(50);
  delay(300);

  openHand();
  setWrist(wristRelaxed);

  WiFi.softAP(ssid, password);

  Serial.println("HAND HOST STARTED");
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Serial.println("WebSocket server ready");
}

void loop() {
  webSocket.loop();

  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == 'o') {
      handOverrideClosed = false;
      openHand();
    }

    if (cmd == 'c') {
      handOverrideClosed = true;
      closeHand();
    }

    if (cmd == 'w') {
      wristState = !wristState;
      setWrist(wristState ? wristFlexed : wristRelaxed);
    }
  }
}