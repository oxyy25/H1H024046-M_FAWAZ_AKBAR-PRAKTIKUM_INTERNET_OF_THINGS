#include <ESP8266WiFi.h>       // Library untuk menghubungkan ESP8266 ke jaringan WiFi
#include <WiFiClientSecure.h>  // Library untuk koneksi jaringan yang menggunakan HTTPS/TLS
#include <PubSubClient.h>      // Library untuk komunikasi dengan broker MQTT
#include <ArduinoJson.h>       // Library untuk membuat dan mengolah data JSON

// Menentukan nama WiFi yang akan digunakan oleh ESP8266
const char *ssid = "nama-wifi";
// Menentukan password WiFi
const char *password = "password";
// Menentukan alamat broker MQTT yang akan digunakan
const char *mqttServer =
    "mqtt.server";
// Menentukan port MQTT
// Port 8883 umumnya digunakan untuk MQTT dengan koneksi TLS/SSL
const int mqttPort = 8883;
// Menentukan username untuk autentikasi ke broker MQTT
const char *mqttUsername = "test";
// Menentukan password untuk autentikasi ke broker MQTT
const char *mqttPassword = "12345678";
// Menentukan topic MQTT tempat data sensor akan dikirim
const char *mqttTopic =
    "test/sensor";
// Membuat objek WiFiClientSecure untuk koneksi yang menggunakan TLS/SSL
WiFiClientSecure espClient;
// Membuat objek MQTT menggunakan koneksi espClient
PubSubClient client(espClient);

// Fungsi untuk menghubungkan ESP8266 ke jaringan WiFi
void hubungkanWiFi()
{
    // Memulai proses koneksi menggunakan SSID dan password yang telah ditentukan
    WiFi.begin(ssid, password);
    // Menampilkan pesan bahwa ESP8266 sedang mencoba terhubung ke WiFi
    Serial.print("Menghubungkan ke WiFi");
    // Selama ESP8266 belum terhubung ke WiFi
    while (WiFi.status() != WL_CONNECTED)
    {
        // Memberikan jeda selama 500 milidetik
        delay(500);
        // Menampilkan tanda titik sebagai indikator proses koneksi
        Serial.print(".");
    }
    // Membuat baris baru pada Serial Monitor
    Serial.println();
    // Menampilkan pesan bahwa koneksi WiFi berhasil
    Serial.println("WiFi berhasil terhubung!");
    // Menampilkan teks IP Address
    Serial.print("IP Address: ");
    // Menampilkan alamat IP yang diberikan oleh jaringan WiFi
    Serial.println(WiFi.localIP());
}


// Fungsi untuk menghubungkan ESP8266 ke broker MQTT
void hubungkanMQTT()
{
    // Selama ESP8266 belum berhasil terhubung dengan broker MQTT
    while (!client.connected())
    {
        // Membuat baris baru pada Serial Monitor
        Serial.println();
        // Menampilkan pesan proses koneksi ke broker MQTT
        Serial.print("Menghubungkan ke broker MQTT...");
        // Membuat ID client MQTT berdasarkan Chip ID ESP8266
        String clientId =
            "ESP8266Client-" +
            String(ESP.getChipId(), HEX);
        // Menampilkan Client ID yang digunakan
        Serial.print(" Client ID: ");
        // Menampilkan Client ID
        Serial.println(clientId);
        // Mencoba menghubungkan ESP8266 ke broker MQTT
        // dengan menggunakan Client ID, username, dan password
        if (client.connect(
                clientId.c_str(),
                mqttUsername,
                mqttPassword))
        {
            // Jika koneksi berhasil, tampilkan pesan berikut
            Serial.println("MQTT berhasil terhubung!");
            // Menampilkan alamat broker MQTT
            Serial.print("Broker : ");
            Serial.println(mqttServer);
            // Menampilkan port MQTT
            Serial.print("Port   : ");
            Serial.println(mqttPort);
            Serial.println("================================");
        }
        else
        {
            // Jika koneksi gagal, tampilkan pesan error
            Serial.print("MQTT gagal, rc=");
            // Menampilkan kode status koneksi MQTT
            Serial.println(client.state());
            // Memberikan informasi bahwa ESP8266 akan mencoba kembali
            Serial.println("Mencoba lagi dalam 2 detik...");
            // Menunggu selama 2 detik sebelum mencoba koneksi kembali
            delay(2000);
        }
    }
}

void setup()
{
    // Memulai komunikasi Serial dengan baud rate 115200
    Serial.begin(115200);
    // Memberikan jeda selama 1 detik setelah Serial dimulai
    delay(1000);
    Serial.println();
    Serial.println("================================");
    // Menampilkan judul program
    Serial.println(" ESP8266 MQTT - HiveMQ Cloud");
    Serial.println("================================");
    // Memanggil fungsi untuk menghubungkan ESP8266 ke WiFi
    hubungkanWiFi();
    // Menonaktifkan pemeriksaan sertifikat TLS/SSL
    espClient.setInsecure();
    // Menentukan alamat broker MQTT dan port yang digunakan
    client.setServer(
        mqttServer,
        mqttPort);
    // Memanggil fungsi untuk menghubungkan ESP8266 ke broker MQTT
    hubungkanMQTT();
}

void loop()
{
    // Memeriksa apakah ESP8266 masih terhubung dengan broker MQTT
    if (!client.connected())
    {
        // Menampilkan baris baru
        Serial.println();
        // Memberikan informasi bahwa koneksi MQTT terputus
        Serial.println("MQTT terputus!");
        // Mencoba menghubungkan kembali ke broker MQTT
        hubungkanMQTT();
    }
    // Menjaga koneksi MQTT tetap aktif
    // dan menangani komunikasi MQTT
    client.loop();
    // Membuat dokumen JSON untuk menyimpan data sensor
    JsonDocument doc;
    // Menambahkan data suhu ke dalam JSON
    doc["suhu"] = 28.5;
    // Menambahkan data kelembaban ke dalam JSON
    doc["kelembaban"] = 65.0;
    // Membuat array karakter untuk menyimpan hasil JSON
    char buffer[128];
    // Mengubah data JSON menjadi format teks
    // kemudian menyimpannya ke dalam buffer
    serializeJson(doc, buffer);
    // Mengirim data JSON ke topic MQTT
    // Hasil pengiriman disimpan dalam variabel berhasil
    bool berhasil =
        client.publish(
            mqttTopic,
            buffer);
    // Memeriksa apakah data berhasil dikirim
    if (berhasil)
    {
        // Menampilkan baris baru
        Serial.println();
        // Menampilkan pesan bahwa data berhasil dikirim
        Serial.println("Data berhasil dikirim!");
        // Menampilkan nama topic MQTT
        Serial.print("Topic   : ");
        // Menampilkan topic yang digunakan
        Serial.println(mqttTopic);
        // Menampilkan teks Payload
        Serial.print("Payload : ");
        // Menampilkan data JSON yang dikirim
        Serial.println(buffer);
    }
    else
    {
        // Menampilkan baris baru
        Serial.println();
        // Menampilkan pesan bahwa pengiriman data gagal
        Serial.println("Gagal mengirim data!");
    }
    // Menunggu selama 5 detik sebelum mengirim data berikutnya
    delay(5000);
}

