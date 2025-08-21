//esp1
#include <ESP8266WiFi.h>
#include <espnow.h>
#include "DHT.h"

#define BUTTON_FLASH 12
#define BUTTON_SINGLE 14
#define BUTTON_MOTOR 16

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

uint8_t slaveAddress[] = {0xC4, 0x5B, 0xBE, 0x70, 0x98, 0xB4};
uint8_t lcdAddress[]   = {0x08, 0xF9, 0xE0, 0x6C, 0x2B, 0x1E};

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Send Status: ");
  Serial.println(sendStatus == 0 ? "Success" : "Fail");
}

int ledCounter = 0;
unsigned long lastDHTTime = 0;
unsigned long motorStartTime = 0;
bool motorRunning = false;
bool motorTriggered = false;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_FLASH, INPUT_PULLUP);
  pinMode(BUTTON_SINGLE, INPUT_PULLUP);
  pinMode(BUTTON_MOTOR, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) return;

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);

  esp_now_add_peer(slaveAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_add_peer(lcdAddress,   ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  dht.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  // FLASH button
  if (digitalRead(BUTTON_FLASH) == LOW) {
    esp_now_send(slaveAddress, (uint8_t *)"FLASH", 5);
    esp_now_send(lcdAddress, (uint8_t *)"FLASH", 5);
    delay(200);
  }

  // SINGLE LED button
  if (digitalRead(BUTTON_SINGLE) == LOW) {
    String msg = "LED" + String(ledCounter);
    esp_now_send(slaveAddress, (uint8_t *)msg.c_str(), msg.length());
    esp_now_send(lcdAddress, (uint8_t *)msg.c_str(), msg.length());
    ledCounter++;
    if (ledCounter > 3) ledCounter = 0;
    delay(200);
  }

  // MOTOR button
  if (digitalRead(BUTTON_MOTOR) == LOW && !motorTriggered) {
    esp_now_send(slaveAddress, (uint8_t *)"MOTOR", 5);
    esp_now_send(lcdAddress, (uint8_t *)"MOTOR", 5);
    motorRunning = true;
    motorStartTime = currentMillis;
    motorTriggered = true; 
    delay(50);
  }

  // تحقق مدة تشغيل الموتور 5 ثواني
  if (motorRunning && currentMillis - motorStartTime >= 5000) {
    motorRunning = false;
    motorTriggered = false; 
  }

  // DHT11 temperature read every 5 seconds
  if (currentMillis - lastDHTTime > 5000) {
    lastDHTTime = currentMillis;
    float temp = dht.readTemperature();
    if (!isnan(temp)) {
      char tempStr[16];
      dtostrf(temp, 4, 1, tempStr);
      String msg = "TEMP:" + String(tempStr);
      esp_now_send(lcdAddress, (uint8_t *)msg.c_str(), msg.length());
    }
  }
}
