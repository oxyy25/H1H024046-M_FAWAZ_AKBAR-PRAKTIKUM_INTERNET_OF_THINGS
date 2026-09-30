#include <ESP8266WiFi.h>      // Library untuk koneksi WiFi ESP8266
#include <PubSubClient.h>      // Library MQTT (publish/subscribe)
#include <WiFiClientSecure.h>  // Client WiFi dengan TLS/SSL (port 8883)
#include <ArduinoJson.h>       // Library untuk parsing & membuat data JSON
#include <DHT.h>               // Library sensor suhu & kelembaban DHT

// Konfigurasi WiFi 
const char *ssid = "Crzx";
const char *password = "CrzxaExe3";

// Konfigurasi MQTT (HiveMQ) 
const char *mqttServer = "broker.hivemq.com";
const int mqttPort = 8883; // Port TLS/SSL, bukan 1883
const char *topicData = "test/data"; // Topic untuk publish data sensor
const char *topicPerintah = "test/sensor"; // Topic untuk subscribe perintah aktuator

// Kredensial MQTT 
const char *mqttUsername = "test";
const char *mqttPassword = "12345678";

// Konfigurasi Hardware Sensor & Aktuator 
#define DHTPIN 2 // Pin data sensor DHT (GPIO2/D4)
#define DHTTYPE DHT11 // Tipe sensor yang dipakai
const int ledPin = 12; // Pin aktuator LED (GPIO12/D6)
DHT dht(DHTPIN, DHTTYPE); // Objek sensor DHT
WiFiClientSecure espClient; // Objek WiFi secure untuk koneksi TLS
PubSubClient client(espClient); // Objek MQTT client berbasis WiFiSecure
unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000; // publish data setiap 5 detik (non-blocking)

// Fungsi callback dipanggil otomatis setiap ada pesan baru masuk
// Parameter: topic = nama topik, payload = isi pesan (byte array), length = panjang pesan
void callback(char *topic, byte *payload, unsigned int length)
{
    // 1. Ubah payload byte-array menjadi String Arduino
    String pesan;
    for (unsigned int i = 0; i < length; i++)
        pesan += (char)payload[i];
    // 2. Deserialisasi JSON, contoh: {"perintah":"ON"}, abaikan jika format salah
    JsonDocument doc;
    if (deserializeJson(doc, pesan))
        return; // abaikan jika parsing gagal
    // 3. Ambil nilai kunci "perintah" dan kendalikan LED (ON = HIGH, selain itu LOW)
    const char *perintah = doc["perintah"];
    digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW);
    Serial.print("Perintah diterima -> Aktuator: ");
    Serial.println(perintah);
}

// Fungsi untuk menghubungkan ESP8266 ke jaringan WiFi
void hubungkanWiFi()
{
    WiFi.begin(ssid, password); // Mulai koneksi dengan SSID & password
    while (WiFi.status() != WL_CONNECTED) // Tunggu sampai status CONNECTED
        delay(500);
    Serial.println("WiFi berhasil terhubung!");
}

// Fungsi untuk menghubungkan ke broker MQTT + subscribe topik perintah
void hubungkanMQTT()
{
    while (!client.connected()) // Ulangi sampai client terhubung
    {
        // Buat clientId acak agar tidak bentrok dengan client lain
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);
        // Hubungkan dengan username & password MQTT
        if (client.connect(clientId.c_str(), mqttUsername, mqttPassword))
        {
            client.subscribe(topicPerintah); // subscribe setelah berhasil terhubung
            Serial.println("Terhubung dan subscribe topic perintah");
        }
        else
        {
            delay(2000); // Tunggu 2 detik sebelum coba lagi
        }
    }
}

// Fungsi setup: dijalankan sekali saat ESP dinyalakan/restart
void setup()
{
    espClient.setInsecure(); // Nonaktifkan verifikasi sertifikat SSL (untuk testing)
    Serial.begin(115200); // Inisialisasi Serial Monitor baudrate 115200
    pinMode(ledPin, OUTPUT); // Atur pin LED sebagai output
    dht.begin(); // Inisialisasi sensor DHT
    hubungkanWiFi(); // Panggil fungsi koneksi WiFi
    client.setServer(mqttServer, mqttPort); // Atur alamat broker & port MQTT
    client.setCallback(callback); // Daftarkan fungsi callback untuk pesan masuk
}

// Fungsi loop: dijalankan berulang terus-menerus (subscribe + publish)
void loop()
{
    if (!client.connected()) // Jika koneksi MQTT putus, sambung ulang
        hubungkanMQTT();
    client.loop(); // Wajib dipanggil terus-menerus agar pesan dapat diterima
    // Publish data sensor secara berkala tanpa memblokir proses subscribe
    if (millis() - waktuTerakhirPublish > intervalPublish) // Cek tiap 5 detik (non-blocking)
    {
        waktuTerakhirPublish = millis(); // Simpan waktu publish terakhir
        float suhu = dht.readTemperature(); // Baca suhu dari sensor DHT
        if (!isnan(suhu)) // Pastikan bacaan valid (bukan NaN)
        {
            JsonDocument doc; // Buat objek JSON untuk payload
            doc["suhu"] = random(24, 30); // Isi data suhu (simulasi 24-29 drajat)
            char buffer[128]; // Buffer char untuk hasil serialisasi JSON
            serializeJson(doc, buffer); // Ubah JSON object -> string, ex: {"suhu":27}
            client.publish(topicData, buffer); // Publish ke topic "test/data"
            Serial.print("Data terkirim: ");
            Serial.println(buffer);
        }
    }
}
