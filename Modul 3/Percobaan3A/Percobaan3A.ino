#include <ESP8266WiFi.h>          // Library untuk menghubungkan ESP8266 ke jaringan WiFi
#include <ESP8266HTTPClient.h>    // Library untuk melakukan komunikasi HTTP
#include <WiFiClientSecure.h>     // Library untuk koneksi HTTPS
#include <ArduinoJson.h>          // Library untuk membuat dan mengolah data JSON

const char *ssid = "wifi-anda";       // Nama jaringan WiFi (SSID)
const char *password = "password";    // Password jaringan WiFi
const char *serverUrl = "https://httpbin.org/post"; // URL server tujuan untuk menerima data

void setup()
{
    Serial.begin(115200); // Memulai komunikasi Serial Monitor dengan baud rate 115200

    WiFi.begin(ssid, password); // Memulai koneksi ESP8266 ke jaringan WiFi menggunakan SSID dan password

    Serial.print("Menghubungkan ke WiFi"); // Menampilkan pesan proses koneksi WiFi
    while (WiFi.status() != WL_CONNECTED) // Mengulang selama ESP8266 belum berhasil terhubung ke WiFi
    {
        delay(500); // Memberikan jeda selama 500 milidetik
        Serial.print("."); // Menampilkan titik sebagai indikator proses koneksi
    }
    Serial.println(); // Membuat baris baru pada Serial Monitor
    Serial.println("WiFi berhasil terhubung!"); // Menampilkan pesan bahwa koneksi WiFi berhasil
}

void loop()
{
    // Memeriksa apakah ESP8266 masih terhubung dengan jaringan WiFi
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http; // Membuat objek HTTPClient untuk komunikasi HTTP

        WiFiClientSecure client; // Membuat client untuk komunikasi HTTPS
        client.setInsecure(); // Menonaktifkan pemeriksaan sertifikat SSL/TLS

        http.begin(client, serverUrl); // Menghubungkan HTTPClient dengan server tujuan
        http.addHeader("Content-Type", "application/json"); // Menentukan bahwa data yang dikirim memiliki format JSON

        JsonDocument doc; // Membuat dokumen JSON untuk menyimpan data

        doc["suhu"] = 28.5; // Menambahkan data suhu sebesar 28.5 ke dalam JSON
        doc["kelembaban"] = 65.0; // Menambahkan data kelembaban sebesar 65.0 ke dalam JSON

        String requestBody; // Membuat variabel String untuk menyimpan data JSON yang akan dikirim

        serializeJson(doc, requestBody); // Mengubah objek JSON menjadi String JSON

        Serial.print("Mengirim data: "); // Menampilkan informasi bahwa data akan dikirim
        Serial.println(requestBody); // Menampilkan isi data JSON yang dikirim

        int httpResponseCode = http.POST(requestBody); // Mengirim data JSON menggunakan metode HTTP POST

        // Memeriksa apakah server memberikan respons dengan kode HTTP yang valid
        if (httpResponseCode > 0)
        {
            Serial.print("Kode Respon HTTP: "); // Menampilkan teks informasi kode respons
            Serial.println(httpResponseCode); // Menampilkan kode respons HTTP dari server

            Serial.println("Isi Respon:"); // Menampilkan teks sebelum isi respons server
            Serial.println(http.getString()); // Mengambil dan menampilkan isi respons dari server
        }
        else
        {
            Serial.print("Pengiriman gagal, kode error: "); // Menampilkan pesan jika pengiriman gagal
            Serial.println(httpResponseCode); // Menampilkan kode error yang diterima
        }

        http.end(); // Mengakhiri koneksi HTTP dan membebaskan resource
    }

    delay(10000); // Menunggu 10 detik sebelum proses pengiriman berikutnya
}

