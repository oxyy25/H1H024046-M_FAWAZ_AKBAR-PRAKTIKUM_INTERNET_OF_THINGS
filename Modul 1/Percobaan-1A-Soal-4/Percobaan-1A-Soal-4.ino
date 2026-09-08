#include <DHT.h>
#define DHTPIN 4      // pin data DHT22 terhubung ke GPIO 4
#define DHTTYPE DHT22 // tipe sensor yang digunakan
DHT dht(DHTPIN, DHTTYPE);

const int JUMLAH_SAMPLING = 5; // jumlah pembacaan yang akan dirata-ratakan

void setup() {
  Serial.begin(115200);
  dht.begin(); // inisialisasi sensor DHT22
  Serial.println("Memulai akuisisi data sensor DHT22...");
}

void loop() {
  float totalSuhu = 0;       // akumulator penjumlahan suhu
  float totalKelembaban = 0; // akumulator penjumlahan kelembaban
  int dataValid = 0;         // penghitung berapa kali pembacaan berhasil (bukan NaN)

  for (int i = 0; i < JUMLAH_SAMPLING; i++) {
    float kelembaban = dht.readHumidity();
    float suhu = dht.readTemperature();

    if (isnan(kelembaban) || isnan(suhu)) {
      Serial.println("Gagal membaca data dari sensor DHT22!");
    } else {
      totalSuhu += suhu;             // menjumlahkan suhu yang valid
      totalKelembaban += kelembaban; // menjumlahkan kelembaban yang valid
      dataValid++;                   // menambah jumlah pembacaan yang berhasil
    }

    delay(2000); // jeda antar pembacaan sensor DHT22 (minimal 2 detik)
  }

  if (dataValid > 0) {
    float rataSuhu = totalSuhu / dataValid;             // menghitung rata-rata suhu
    float rataKelembaban = totalKelembaban / dataValid; // menghitung rata-rata kelembaban

    Serial.print("Rata-rata Suhu: ");
    Serial.print(rataSuhu);
    Serial.print(" °C, Rata-rata Kelembaban: ");
    Serial.print(rataKelembaban);
    Serial.println(" %");
  } else {
    Serial.println("Semua pembacaan gagal, tidak ada data untuk dirata-ratakan.");
  }
}
