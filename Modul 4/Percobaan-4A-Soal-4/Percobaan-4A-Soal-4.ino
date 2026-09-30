#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* topicPerintah = "unsoed/tk245004/kelompokAnda/perintah";

const int ledPin = 26;

// TAMBAHAN: konfigurasi PWM 
const int pwmFreq = 5000;      // frekuensi PWM 5 kHz
const int pwmResolusi = 8;     // resolusi 8 bit -> nilai duty 0-255

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }
  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(pesan);

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, pesan);
  if (error) {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // TAMBAHAN: cek key "perintah" ada atau tidak 
  if (doc["perintah"].isNull()) {
    Serial.println("Key 'perintah' tidak ditemukan");
    return;
  }

  const char* perintah = doc["perintah"];
  // TAMBAHAN: baca intensitas, default 255 bila tidak dikirim 
  int intensitas = doc["intensitas"] | 255;
  // TAMBAHAN: batasi nilai agar tetap dalam rentang 0-255 
  intensitas = constrain(intensitas, 0, 255);

  if (String(perintah) == "ON") {
    ledcWrite(ledPin, intensitas);  // DIUBAH: PWM menggantikan digitalWrite HIGH 
    Serial.print("Aktuator: ON, intensitas = ");
    Serial.println(intensitas);
  } else if (String(perintah) == "OFF") {
    ledcWrite(ledPin, 0);  // DIUBAH: duty 0 = LED mati 
    Serial.println("Aktuator: OFF");
  }
}

void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke broker MQTT...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
      client.subscribe(topicPerintah);
      Serial.print("Subscribe ke topic: ");
      Serial.println(topicPerintah);
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // DIUBAH: pinMode diganti setup PWM 
  ledcAttach(ledPin, pwmFreq, pwmResolusi);
  ledcWrite(ledPin, 0);  // LED mati saat awal

  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    hubungkanMQTT();
  }
  client.loop();
}
