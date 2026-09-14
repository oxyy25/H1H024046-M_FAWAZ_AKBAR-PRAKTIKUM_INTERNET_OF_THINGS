#include <ESP8266WiFi.h>

// Kredensial untuk mode Station (menghubungkan ke WiFi rumah) 
const char* sta_ssid     = "NAMA_WIFI_RUMAH_ANDA";
const char* sta_password = "PASSWORD_WIFI_RUMAH_ANDA";

// Kredensial untuk mode Access Point (yang disediakan ESP32) 
const char* ap_ssid     = "ESP32_AccessPoint";
const char* ap_password = "12345678"; // minimal 8 karakter

const int ledPin = 2;   // LED indikator status koneksi STA

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Set mode WiFi menjadi AP+STA (gabungan Access Point dan Station) 
  WiFi.mode(WIFI_AP_STA);

  // Bagian Access Point: aktifkan hotspot ESP32 
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress apIP = WiFi.softAPIP();

  Serial.println("Access Point aktif!");
  Serial.print("AP SSID       : ");
  Serial.println(ap_ssid);
  Serial.print("AP IP Address : ");
  Serial.println(apIP);

  // Bagian Station: mulai koneksi ke WiFi rumah 
  WiFi.begin(sta_ssid, sta_password);
  Serial.print("Menghubungkan ke WiFi rumah");

  unsigned long startAttempt = millis();
  // Batasi waktu tunggu koneksi STA maksimal 15 detik agar AP tetap bisa
  // diakses meskipun koneksi ke WiFi rumah belum/tidak berhasil
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("STA berhasil terhubung ke WiFi rumah!");
    Serial.print("STA IP Address  : ");
    Serial.println(WiFi.localIP());
    Serial.print("STA MAC Address : ");
    Serial.println(WiFi.macAddress());
    Serial.print("STA RSSI (dBm)  : ");
    Serial.println(WiFi.RSSI());
    digitalWrite(ledPin, HIGH);
  } else {
    Serial.println("STA gagal terhubung ke WiFi rumah (AP tetap aktif).");
    digitalWrite(ledPin, LOW);
  }
}

void loop() {
  // Pantau status koneksi STA setiap 5 detik
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Status STA: Terhubung ke WiFi rumah");
    digitalWrite(ledPin, HIGH);
  } else {
    Serial.println("Status STA: Terputus dari WiFi rumah");
    digitalWrite(ledPin, LOW);
  }

  // Pantau jumlah perangkat yang terhubung ke AP ESP32 
  int jumlahClient = WiFi.softAPgetStationNum();
  Serial.print("Jumlah perangkat terhubung ke AP: ");
  Serial.println(jumlahClient);

  delay(5000);
}
