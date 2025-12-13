#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

volatile bool newData = false;
volatile uint8_t latestValue = 0;

void onDataReceived(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  if (len >= 1) {
    latestValue = incomingData[0];
    newData = true;
  }
}

void setup() {
  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println("Waiting...");
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW init failed");
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onDataReceived);
}

void loop() {
  if (newData) {
    newData = false;

    Serial.print("Received: ");
    Serial.println(latestValue);

    display.clearDisplay();
    display.setTextSize(3);
    display.setCursor(0, 20);
    display.print(latestValue);
    display.display();
  }
}
