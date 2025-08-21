//esp3
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <LiquidCrystal.h>

const int rs = 5;
const int en = 4;
const int d4 = 14;
const int d5 = 12;
const int d6 = 13;
const int d7 = 15;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

String receivedData = "";

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  receivedData = "";
  for (int i = 0; i < len; i++) {
    if (incomingData[i] >= 32 && incomingData[i] <= 126) {
      receivedData += (char)incomingData[i];
    }
  }

  Serial.print("Data Received: ");
  Serial.println(receivedData);

  lcd.clear();
  lcd.setCursor(0, 0);

  if (receivedData.startsWith("TEMP:")) {
    lcd.print("Temp: ");
    lcd.print(receivedData.substring(5));
    lcd.print((char)223);
    lcd.print("C");
  } else {
    lcd.print(receivedData);
  }
}

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.print("Waiting data...");

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) return;

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
}
