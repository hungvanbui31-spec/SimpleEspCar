#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Arduino.h>


struct DataPacket {
  int16_t adc0, adc1;
  uint8_t status;
};
DataPacket packet;

void onDataReceived(uint8_t *mac, uint8_t *data, uint8_t len) {
  if (len == sizeof(DataPacket)) {
    memcpy(&packet, data, sizeof(packet));

    Serial.print("Value1: ");
    Serial.println(packet.adc0);

    Serial.print("Value2: ");
    Serial.println(packet.adc1);

    Serial.print("Status: ");
    Serial.println(packet.status);
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW init failed");
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onDataReceived);
}

void loop() {
}
