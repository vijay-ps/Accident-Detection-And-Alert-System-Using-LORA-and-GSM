/************************************************************
 * Accident Broadcast Receiver – LoRa 433 MHz
 ************************************************************/

#include <SPI.h>
#include <LoRa.h>

#define LORA_SCK   36
#define LORA_MISO  37
#define LORA_MOSI  35
#define LORA_CS    34
#define LORA_RST   33
#define LORA_DIO0  38

void setup() {
  Serial.begin(115200);

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("❌ LoRa Startup Failed!");
    while (1);
  }

  Serial.println("🛰 LoRa Accident Receiver Ready");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (!packetSize) return;

  String data = "";
  while (LoRa.available()) data += (char)LoRa.read();

  Serial.println("\n🚨 Accident Message:");
  Serial.println(data);
  Serial.print("RSSI: ");
  Serial.println(LoRa.packetRssi());

  // OPTIONAL: Sound buzzer / LED alert
}
