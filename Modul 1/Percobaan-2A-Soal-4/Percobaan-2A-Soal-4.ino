#include <DHT.h>
#define DHTPIN 4      // pin data DHT22 terhubung ke GPIO 4
#define DHTTYPE DHT22
#define RELAYPIN 26   // pin kendali relay/LED indikator
DHT dht(DHTPIN, DHTTYPE);

const float suhuNyala = 30.0; // ambang batas atas: aktuator menyala jika suhu melewati ini
const float suhuMati = 28.0;  // ambang batas bawah: aktuator mati jika suhu turun di bawah ini

bool statusAktuator = false; // menyimpan status aktuator saat ini (true = ON, false = OFF)

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW); // pastikan aktuator mati di awal
}

void loop() {
  float suhu = dht.readTemperature();

  if (isnan(suhu)) {
    Serial.println("Gagal membaca data sensor!");
  } else {
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C -> ");

    // Kendali aktuator berbasis histerisis (dua ambang batas)
    if (suhu > suhuNyala) {
      statusAktuator = true;  // suhu melewati batas atas, aktuator dinyalakan
    } else if (suhu < suhuMati) {
      statusAktuator = false; // suhu turun di bawah batas bawah, aktuator dimatikan
    }
    // jika suhu berada di antara suhuMati dan suhuNyala, statusAktuator tidak diubah

    digitalWrite(RELAYPIN, statusAktuator ? HIGH : LOW); // menerapkan status ke pin relay

    Serial.println(statusAktuator ? "Aktuator: ON" : "Aktuator: OFF");
  }

  delay(2000); // jeda pembacaan sensor DHT22 (minimal 2 detik)
}
