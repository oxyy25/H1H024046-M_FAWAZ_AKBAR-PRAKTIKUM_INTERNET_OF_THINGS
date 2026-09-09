#include <ESP8266WiFi.h>

const char *ap_ssid = "ESP32_AccessPointc"; //SSID ESP8266
const char *ap_password = "12345678"; // minimal 8 karakter

void setup()
{
    Serial.begin(115200);

    // Set mode WiFi menjadi Access Point
    WiFi.mode(WIFI_AP);
     // Mengaktifkan Access Point dengan SSID dan password
    WiFi.softAP(ap_ssid, ap_password);
    // Mengambil alamat IP dari Access Point yang baru dibuat.
    // Nilainya secara default biasanya 192.168.4.1
    IPAddress apIP = WiFi.softAPIP();
    Serial.println("Access Point aktif!");
    Serial.print("SSID          : "); // cektak nama SSID dari Access Point
    Serial.println(ap_ssid);
    Serial.print("IP Address    : "); // cektak alamat IP dari Access Point
    Serial.println(apIP);
}

void loop()
{
    // Menampilkan jumlah perangkat yang terhubung setiap 5 detik
    int jumlahClient = WiFi.softAPgetStationNum();
    Serial.print("Jumlah perangkat terhubung: ");
    Serial.println(jumlahClient);
    delay(5000);
}