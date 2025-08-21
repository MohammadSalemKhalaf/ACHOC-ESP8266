//esp2
#include <ESP8266WiFi.h>
#include <espnow.h>

#define LED0 16
#define LED1 5
#define LED2 4
#define MOTOR_PIN 12

unsigned long motorStartTime = 0;
bool motorRunning = false;

void flashLEDs() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(LED0, HIGH);
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    delay(300);
    digitalWrite(LED0, LOW);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    delay(300);
  }
}

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  String msg = "";
  for (int i = 0; i < len; i++) msg += (char)incomingData[i];

  if (msg == "FLASH") {
    flashLEDs();
  } 
  else if (msg.startsWith("LED")) {
    int ledNum = msg.charAt(3) - '0';
    int ledPins[] = {LED0, LED1, LED2};
    if (ledNum >= 0 && ledNum <= 2) {
      digitalWrite(ledPins[ledNum], HIGH);
      delay(500);
      digitalWrite(ledPins[ledNum], LOW);
    }
  }
  else if (msg == "MOTOR") {
    if (!motorRunning) { 
      digitalWrite(MOTOR_PIN, HIGH);
      motorStartTime = millis();
      motorRunning = true;
      Serial.println("Motor Started");
    } else {
      Serial.println("Motor is already running, ignoring new command");
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) return;

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  if (motorRunning && millis() - motorStartTime >= 5000) { 
    digitalWrite(MOTOR_PIN, LOW);
    motorRunning = false;
    Serial.println("Motor Stopped");
  }
}
