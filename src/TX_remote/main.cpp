#include <ESP8266WiFi.h>
#include <Adafruit_ADS1X15.h>
#include <Arduino.h>
#include <espnow.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define modules
Adafruit_ADS1115 ads;
Adafruit_SSD1306 display(128, 64, &Wire, -1);
// -----------------------------------
// Receiver MAC
uint8_t receiverMAC[] = {0x24, 0xD7, 0xEB, 0xEB, 0xFA, 0x81};

//int16_t adc0, adc1;

const int boxSize = 50;                 // Size of the square in pixels
const int boxX0 = (128  - boxSize) / 2;
const int boxY0 = (64 - boxSize) / 2;

struct DataPacket {
  int16_t adc0, adc1;
  uint8_t status;
};
DataPacket packet;

void onSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Delivery Status: ");
  Serial.println(sendStatus == 0 ? "Success" : "Fail");
}

void drawJoystickPosition(int mappedX, int mappedY) {

  display.clearDisplay();

  // Draw bounding square
  display.drawRect(boxX0, boxY0, boxSize, boxSize, SSD1306_WHITE);

  // Draw the dot (use a small filled circle)
  display.fillCircle(mappedX, mappedY, 2, SSD1306_WHITE);

  display.display();
}

void setup() {
  Serial.begin(115200);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
  Serial.println("SSD1306 allocation failed");
  while (true);
}

  ads.begin();
  ads.setGain(GAIN_TWOTHIRDS);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Register peer
  esp_now_add_peer(receiverMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  Serial.println("ESP-NOW Sender Ready");
}

void loop() {
  const int32_t ads_max = (5.03/6.144)*32768;

  int16_t raw0 = ads.readADC_SingleEnded(0);
  int16_t raw1 = ads.readADC_SingleEnded(1);

  packet.adc0 = map(raw0,0,(5.03/6.144)*32768,0,100);
  packet.adc1 = map(raw1,0,(5.03/6.144)*32768,0,100);
  packet.status  = (packet.adc0 > 50);

  int posX = map(raw0,0,ads_max, boxX0, boxX0 + boxSize - 1);
  int posY = map(raw1,0,ads_max, boxY0, boxY0 + boxSize - 1);

  drawJoystickPosition(posX, posY);

  esp_now_send(receiverMAC, (uint8_t*) &packet, sizeof(packet));

  delay(50); // Adjust sending rate as needed
}

