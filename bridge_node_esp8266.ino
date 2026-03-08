#include <ESP8266WiFi.h>
#include <espnow.h>

#define STATUS_LED 2 // Onboard LED

uint8_t broadcastAddress[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

typedef struct {
  char json[200];
} Packet;

Packet incoming;

unsigned long ledOffTime = 0;
const int blinkDuration = 80;

void OnDataRecv(uint8_t *mac, uint8_t *data, uint8_t len) {
  digitalWrite(STATUS_LED, LOW);
  ledOffTime = millis() + blinkDuration;

  memcpy(&incoming, data, sizeof(incoming));

  Serial.print("Received & Rebroadcasting: ");
  Serial.println(incoming.json);

  esp_now_send(broadcastAddress, (uint8_t*)&incoming, sizeof(incoming));
}

void updateLED() {
  if (millis() >= ledOffTime && digitalRead(STATUS_LED) == LOW) {
    digitalWrite(STATUS_LED, HIGH);
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, HIGH);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

  Serial.println("Bridge node initialized.");
}

void loop() {
  updateLED();
  
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 10000) {
    Serial.println("Status: Waiting for packets...");
    lastHeartbeat = millis();
  }
}
