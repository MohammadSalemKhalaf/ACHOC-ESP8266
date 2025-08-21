#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ThingESP.h>

ESP8266WiFiMulti wifiMulti;
ThingESP8266 thing("mbaboshi4", "ledcontrol", "led12345");

#define LED1 16
#define LED2 5
#define LED3 4
#define LED4 0
#define MOTOR_OPEN_PIN 12
#define MOTOR_CLOSE_PIN 13

void setup() {
  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(MOTOR_OPEN_PIN, OUTPUT);
  pinMode(MOTOR_CLOSE_PIN, OUTPUT);

  turnOffAll();
  stopMotor();

  wifiMulti.addAP("Bashar", "azah1342");
  wifiMulti.addAP("hamode aboshi", "12345678");

  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  thing.initDevice();
  thing.setCallback(HandleResponse);
}

String HandleResponse(String query) {
  query.toLowerCase();
  Serial.println("Received: " + query);

  if (query == "1") {
    turnOffAll();
    digitalWrite(LED1, HIGH);
    return "LED1 ON";
  }
  else if (query == "2") {
    turnOffAll();
    digitalWrite(LED2, HIGH);
    return "LED2 ON";
  }
  else if (query == "3") {
    turnOffAll();
    digitalWrite(LED3, HIGH);
    return "LED3 ON";
  }
  else if (query == "4") {
    turnOffAll();
    digitalWrite(LED4, HIGH);
    return "LED4 ON";
  }
  else if (query == "flash") {
    flashLeds();
    return "Flash Complete!";
  }
  else if (query == "counter") {
    counterLeds();
    return "Binary Counter Complete!";
  }
  else if (query == "on all") {
    turnOnAll();
    return "All LEDs ON";
  }
  else if (query == "off all") {
    turnOffAll();
    return "All LEDs OFF";
  }
  else if (query == "test") {
    turnOffAll();
    return "System Working Fine!";
  }
  else if (query == "run motor") {
    turnOffAll();
    motorRun();
    return "Motor test running!";
  }
  else if (query == "stop motor") {
    turnOffAll();
    stopMotor();
    return "Motor stopped!";
  }

  return "Invalid command";
}

void loop() {
  thing.Handle();
  delay(10);
}

void turnOffAll() {
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
}

void turnOnAll() {
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
}

void flashLeds() {
  for (int i = 0; i < 10; i++) {
    turnOnAll();
    delay(300);
    turnOffAll();
    delay(300);
  }
}

void counterLeds() {
  for (int count = 0; count <= 15; count++) {
    digitalWrite(LED1, (count & 0b0001) ? HIGH : LOW);
    digitalWrite(LED2, (count & 0b0010) ? HIGH : LOW);
    digitalWrite(LED3, (count & 0b0100) ? HIGH : LOW);
    digitalWrite(LED4, (count & 0b1000) ? HIGH : LOW);
    delay(500);
  }
  turnOffAll();
}

void stopMotor() {
  digitalWrite(MOTOR_OPEN_PIN, LOW);
  digitalWrite(MOTOR_CLOSE_PIN, LOW);
}

void motorRun() {
  static unsigned long lastTime = 0;
  static bool motorOn = false;

  unsigned long currentTime = millis();

  if (!motorOn && currentTime - lastTime >= 15000) {
    digitalWrite(MOTOR_OPEN_PIN, HIGH);
    motorOn = true;
    lastTime = currentTime;
  }

  if (motorOn && currentTime - lastTime >= 5000) {
    digitalWrite(MOTOR_OPEN_PIN, LOW);
    motorOn = false;
    lastTime = currentTime;
  }
}