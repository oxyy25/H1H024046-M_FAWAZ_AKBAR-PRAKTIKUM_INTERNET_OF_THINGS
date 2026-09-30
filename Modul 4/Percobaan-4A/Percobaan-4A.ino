#include <ESP8266WiFi.h>      // Library untuk koneksi WiFi ESP8266
#include <WiFiClientSecure.h>  // Client WiFi dengan TLS/SSL (port 8883)
#include <PubSubClient.h>      // Library MQTT (publish/subscribe)
#include <ArduinoJson.h>       // Library untuk parsing data JSON

// Konfigurasi WiFi 
const char *ssid = "nama-wifi";
const char *password = "password";

// Konfigurasi MQTT (HiveMQ) 
const char *mqttServer = "broker.hivemq.com";
const int mqttPort = 8883; // Port TLS/SSL
const char *topicPerintah = "test/sensor"; // Topic yang di-subscribe

// Kredensial MQTT & Hardware 
const char *mqttUsername = "test";
const char *mqttPassword = "12345678";
const int ledPin = 4; // Pin aktuator LED (GPIO4/D2)
WiFiClientSecure espClient; // Objek WiFi secure untuk koneksi TLS

PubSubClient client(espClient); // Objek MQTT client berbasis WiFiSecure

// Fungsi callback dipanggil otomatis setiap ada pesan baru masuk
// Parameter: topic = nama topik, payload = isi pesan (byte array), length = panjang pesan
void callback(char *topic, byte *payload, unsigned int length)
{
    // 1. Ubah payload byte-array menjadi String Arduino
    String pesan;
    for (unsigned int i = 0; i < length; i++)
    {
        pesan += (char)payload[i];
    }
    // 2. Tampilkan pesan mentah untuk debugging di Serial Monitor
    Serial.print("Pesan diterima [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(pesan);
    // 3. Deserialisasi data JSON yang diterima, contoh: {"perintah":"ON"}
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, pesan);
    if (error)
    {
        Serial.print("Gagal parsing JSON: ");
        Serial.println(error.c_str());
        return; // Keluar jika format JSON salah
    }
    // 4. Ambil nilai kunci "perintah" dan kendalikan aktuator LED
    const char *perintah = doc["perintah"];
    if (String(perintah) == "ON")
    {
        digitalWrite(ledPin, HIGH); // Nyalakan LED
        Serial.println("Aktuator: ON");
    }
    else if (String(perintah) == "OFF")
    {
        digitalWrite(ledPin, LOW); // Matikan LED
        Serial.println("Aktuator: OFF");
    }
}

// Fungsi untuk menghubungkan ESP8266 ke jaringan WiFi
void hubungkanWiFi()
{
    WiFi.begin(ssid, password); // Mulai koneksi dengan SSID & password
    Serial.print("Menghubungkan ke WiFi");
    while (WiFi.status() != WL_CONNECTED) // Tunggu sampai status CONNECTED
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi berhasil terhubung!");
}

// Fungsi untuk menghubungkan ke broker MQTT + subscribe topik
void hubungkanMQTT()
{
    while (!client.connected()) // Ulangi sampai client terhubung
    {
        Serial.print("Menghubungkan ke broker MQTT...");
        // Buat clientId acak agar tidak bentrok dengan client lain
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);
        // Hubungkan dengan username & password MQTT
        if (client.connect(clientId.c_str(), mqttUsername, mqttPassword))
        {
            Serial.println("berhasil terhubung!");
            client.subscribe(topicPerintah); // subscribe setelah berhasil terhubung
            Serial.print("Subscribe ke topic: ");
            Serial.println(topicPerintah);
        }
        else
        {
            // Jika gagal, tampilkan kode error dan coba lagi 2 detik
            Serial.print("gagal, rc=");
            Serial.print(client.state());
            Serial.println(" coba lagi dalam 2 detik");
            delay(2000);
        }
    }
}

// Fungsi setup: dijalankan sekali saat ESP dinyalakan/restart
void setup()
{
    espClient.setInsecure(); // Nonaktifkan verifikasi sertifikat SSL (untuk testing)
    Serial.begin(115200); // Inisialisasi Serial Monitor baudrate 115200
    pinMode(ledPin, OUTPUT); // Atur pin LED sebagai output
    digitalWrite(ledPin, LOW); // Kondisi awal LED mati
    hubungkanWiFi(); // Panggil fungsi koneksi WiFi
    client.setServer(mqttServer, mqttPort); // Atur alamat broker & port MQTT
    client.setCallback(callback); // Daftarkan fungsi callback untuk pesan masuk
}

// Fungsi loop: dijalankan berulang terus-menerus
void loop()
{
    if (!client.connected()) // Jika koneksi MQTT putus, sambung ulang
    {
        hubungkanMQTT();
    }
    client.loop(); // Wajib dipanggil terus-menerus agar pesan dapat diterima
}
