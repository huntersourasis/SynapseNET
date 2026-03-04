#include <WiFi.h>
#include <esp_now.h>

uint8_t broadcastAddress[] = {0xff,0xff,0xff,0xff,0xff,0xff};

typedef struct {
  char json[200];
} Packet;

Packet packet;

void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  esp_now_add_peer(&peerInfo);

  Serial.println("Sensor node started");
}

void loop() {

  strcpy(packet.json,
  "{\"node\":\"sensor1\",\"temp\":25.3,\"hum\":60}");

  esp_now_send(broadcastAddress,(uint8_t*)&packet,sizeof(packet));

  Serial.println(packet.json);

  delay(2000);
}
