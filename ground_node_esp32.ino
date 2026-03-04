#include <WiFi.h>
#include <esp_now.h>

typedef struct {
  char json[200];
} Packet;

Packet incoming;

void OnDataRecv(const esp_now_recv_info_t *info,const uint8_t *data,int len){

  memcpy(&incoming,data,sizeof(incoming));

  Serial.print("Telemetry: ");
  Serial.println(incoming.json);
}

void setup(){

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if(esp_now_init()!=ESP_OK){
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Ground station ready");
}

void loop(){
}
