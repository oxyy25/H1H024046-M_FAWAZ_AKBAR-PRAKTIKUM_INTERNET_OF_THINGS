#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

const char* ssid = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

const char* topicData = "unsoed/tk245004/kelompokAnda/data";
const char* topicPerintah = "unsoed/tk245004/kelompokAnda/perintah";
// TAMBAHAN: topic untuk aktuator kedua (buzzer) 
const char* topicBuzzer = "unsoed/tk245004/kelompokAnda/buzzer";

#define DHTPIN 4
#define DHTTYPE DHT22
const int ledPin = 26;
// TAMBAHAN: pin buzzer
const int buzzerPin = 27;

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000;

void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (unsigned int i = 0; i < length; i++) pesan += (char)payload[i];

  JsonDocument doc;
  if (deserializeJson(doc, pesan)) return;

  // TAMBAHAN: cek key perintah agar tidak null 
  if (doc["perintah"].isNull()) return;
  const char* perintah = doc["perintah"];
  bool nyala = (String(perintah) == "ON");

  // DIUBAH: callback membedakan topic penerima pesan 
  if (strcmp(topic, topicPerintah) == 0) {
    digitalWrite(ledPin, nyala ? HIGH : LOW);
    Serial.print("[LED] Perintah diterima -> ");
    Serial.println(perintah);
  } else if (strcmp(topic, topicBuzzer) == 0) {
    digitalWrite(buzzerPin, nyala ? HIGH : LOW);
    Serial.print("[BUZZER] Perintah diterima -> ");
    Serial.println(perintah);
  }
}

void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe(topicPerintah);
      // TAMBAHAN: subscribe topic buzzer 
      client.subscribe(topicBuzzer);
      Serial.println("Terhubung dan subscribe topic perintah + buzzer");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  // TAMBAHAN: pin buzzer sebagai output, awalnya mati 
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  dht.begin();
  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) hubungkanMQTT();
  client.loop();

  if (millis() - waktuTerakhirPublish > intervalPublish) {
    waktuTerakhirPublish = millis();
    float suhu = dht.readTemperature();
    if (!isnan(suhu)) {
      JsonDocument doc;
      doc["suhu"] = suhu;
      char buffer[128];
      serializeJson(doc, buffer);
      client.publish(topicData, buffer);
      Serial.print("Data terkirim: ");
      Serial.println(buffer);
    }
  }
}
