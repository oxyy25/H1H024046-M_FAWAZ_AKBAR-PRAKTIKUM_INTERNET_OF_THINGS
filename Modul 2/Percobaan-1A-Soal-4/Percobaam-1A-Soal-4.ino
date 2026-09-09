#include <ESP8266WiFi.h>

const char* ssid     = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

const int ledPin = 2;   // LED indikator status koneksi

// Tambahan untuk fitur auto-reconnect 
unsigned long previousReconnectMillis = 0;      // Waktu terakhir mencoba reconnect
const unsigned long reconnectInterval = 5000;   // Jarak antar percobaan reconnect (ms)


void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Set mode WiFi menjadi Station
  WiFi.mode(WIFI_STA);

  // aktifkan auto-reconnect bawaan library WiFi 
  // Jika koneksi terputus, ESP32 akan otomatis mencoba menyambung
  // kembali menggunakan SSID/password terakhir yang digunakan.
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);


  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan ke WiFi");
  unsigned long startAttempt = millis();

  // ---- Tambahan: batasi waktu tunggu awal (timeout) agar tidak hang selamanya ----
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
    delay(500);
    Serial.print(".");
  }
  

  if (WiFi.status() == WL_CONNECTED) {
    // Jika berhasil terhubung
    Serial.println();
    Serial.println("WiFi berhasil terhubung!");
    Serial.print("IP Address  : ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC Address : ");
    Serial.println(WiFi.macAddress());
    Serial.print("RSSI (dBm)  : ");
    Serial.println(WiFi.RSSI());

    digitalWrite(ledPin, HIGH); // nyalakan LED sebagai indikator
  } else {
    // Tambahan: penanganan jika gagal konek saat pertama kali 
    Serial.println();
    Serial.println("Gagal terhubung dalam waktu yang ditentukan, akan dicoba lagi di loop().");
    digitalWrite(ledPin, LOW);
    
  }
}

void loop() {
  // Cek status koneksi setiap 5 detik
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Status: Terhubung");
    digitalWrite(ledPin, HIGH);
  } else {
    Serial.println("Status: Terputus");
    digitalWrite(ledPin, LOW);

    // logika reconnect otomatis 
    unsigned long currentMillis = millis();
    // Cek apakah sudah waktunya mencoba reconnect lagi
    // (mencegah pemanggilan WiFi.reconnect() terlalu sering / spam)
    if (currentMillis - previousReconnectMillis >= reconnectInterval) {
      previousReconnectMillis = currentMillis;
      Serial.println("Mencoba menghubungkan ulang ke WiFi...");
      WiFi.disconnect();     // Pastikan koneksi lama dibersihkan dahulu
      WiFi.reconnect();      // Coba sambungkan kembali menggunakan kredensial terakhir
    }
  }

  delay(5000);
}
