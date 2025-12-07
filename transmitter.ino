/************************************************************
 *  Accident Detection & LoRa Broadcast (Sender – ESP32-S2)
 *  - MPU6050 (I2C)
 *  - GPS Neo-6M (UART1)
 *  - LoRa SX1278 (433 MHz)
 *  - WiFi AP Web UI for Car Number Setup
 ************************************************************/

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <SPI.h>
#include <LoRa.h>
#include <math.h>

// ===== PIN CONFIG =====
#define SDA_PIN 8
#define SCL_PIN 9
#define GPS_RX 4
#define GPS_TX 5
#define LORA_SCK   36
#define LORA_MISO  37
#define LORA_MOSI  35
#define LORA_CS    34
#define LORA_RST   33
#define LORA_DIO0  38

Adafruit_MPU6050 mpu;
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
Preferences prefs;

WebServer server(80);
String carNumber = "CAR000";

// ===== THRESHOLDS =====
#define LEVEL1_JERK            15.0
#define LEVEL2_SPEED_BREAKER   16.0
#define LEVEL3_SHARP_TURN      19.0
#define LEVEL4_FALL            23.0
#define LEVEL5_ACCIDENT        28.0

#define CHECK_INTERVAL         80
#define ALERT_COOLDOWN         6000
unsigned long lastAlert = 0;

String fallbackLoc = "https://www.google.com/maps?q=12.842359,80.156692";

// ===== GET GPS LOCATION =====
String getLocation() {
  while (gpsSerial.available()) gps.encode(gpsSerial.read());

  if (gps.location.isValid()) {
    return "https://maps.google.com/?q=" +
           String(gps.location.lat(), 6) + "," +
           String(gps.location.lng(), 6);
  }
  return fallbackLoc;
}

// ===== WEB UI =====
void handleRoot() {
  String html =
    "<html><body style='font-family:Arial'>"
    "<h2>🚗 Configure Car Number</h2>"
    "<form action='/save'>"
    "<input name='num' value='" + carNumber + "'>"
    "<br><br><button type='submit'>Save</button>"
    "</form></body></html>";

  server.send(200, "text/html", html);
}

void handleSave() {
  carNumber = server.arg("num");
  prefs.putString("car", carNumber);
  server.send(200, "text/html",
              "<h3>✅ Car Number Saved! Restart Device.</h3>");
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  delay(1000);

  prefs.begin("carData", false);
  carNumber = prefs.getString("car", carNumber);

  Wire.begin(SDA_PIN, SCL_PIN);
  if (!mpu.begin(0x68, &Wire)) {
    Serial.println("❌ MPU6050 not detected!");
    while (1);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  gpsSerial.begin(9600, SERIAL_8N1, GPS_TX, GPS_RX);

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("❌ LoRa Initialization Failed!");
    while (1);
  }

  WiFi.softAP("ESP32-CarSetup", "12345678");
  server.on("/", handleRoot);
  server.on("/save", HTTP_GET, handleSave);
  server.begin();

  Serial.println("\n🌐 WiFi AP: ESP32-CarSetup");
  Serial.println("http://192.168.4.1 to set Car Number\n");
}

// ===== LOOP =====
void loop() {
  server.handleClient();
  while (gpsSerial.available()) gps.encode(gpsSerial.read());

  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);

  float acc = sqrt(a.acceleration.x * a.acceleration.x +
                   a.acceleration.y * a.acceleration.y +
                   a.acceleration.z * a.acceleration.z);

  int level = 0;

  if (acc > LEVEL1_JERK && acc <= LEVEL2_SPEED_BREAKER) level = 1;
  else if (acc > LEVEL2_SPEED_BREAKER && acc <= LEVEL3_SHARP_TURN) level = 2;
  else if (acc > LEVEL3_SHARP_TURN && acc <= LEVEL4_FALL) level = 3;
  else if (acc > LEVEL4_FALL && acc <= LEVEL5_ACCIDENT) level = 4;
  else if (acc > LEVEL5_ACCIDENT) level = 5;

  if (level > 0) {
    Serial.printf("⚠️ Level %d | Acc: %.2f\n", level, acc);
  }

  if (level == 5 && (millis() - lastAlert) > ALERT_COOLDOWN) {

    String location = getLocation();

    String jsonMsg =
      "{"
      "\"car\":\"" + carNumber + "\","
      "\"level\":5,"
      "\"type\":\"ACCIDENT\","
      "\"acc\":" + String(acc, 2) + ","
      "\"loc\":\"" + location + "\""
      "}";

    LoRa.beginPacket();
    LoRa.print(jsonMsg);
    LoRa.endPacket();

    Serial.println("\n🚨 Accident Broadcast Sent:");
    Serial.println(jsonMsg);

    lastAlert = millis();
  }

  // LISTEN FOR OTHER BROADCASTS
  int packet = LoRa.parsePacket();
  if (packet) {
    String incoming = "";
    while (LoRa.available()) incoming += (char)LoRa.read();

    Serial.println("\n📥 Incoming Broadcast:");
    Serial.println(incoming);
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());
  }

  delay(CHECK_INTERVAL);
}
