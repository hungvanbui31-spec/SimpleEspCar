#include <ESP8266WiFi.h>
#include <espnow.h>

// Replace with your receiver ESP8266 MAC address
uint8_t receiverMAC[] = {0x24, 0xD7, 0xEB, 0xEB, 0xFA, 0x81};

int valueToSend = 0;

void onSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Delivery Status: ");
  Serial.println(sendStatus == 0 ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);

  // ESP-NOW requires Wi-Fi to be in station mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Register peer
  esp_now_add_peer(receiverMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // Optional: callback when packet is sent
  esp_now_register_send_cb(onSent);

  Serial.println("ESP-NOW Sender Ready");
}

void loop() {
  // Send the integer value
  uint8_t data = valueToSend;
  esp_now_send(receiverMAC, &data, sizeof(data));

  Serial.print("Sent: ");
  Serial.println(valueToSend);

  // Increase value
  valueToSend++;
  if (valueToSend > 100) {
    valueToSend = 0;
  }

  delay(200); // Adjust sending rate as needed
}
