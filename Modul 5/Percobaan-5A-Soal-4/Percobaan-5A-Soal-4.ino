#include <DHT.h>
#include <math.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

const int N = 5;     // jumlah sampel moving average
float buffer[N];     // menyimpan N pembacaan terakhir
int indeks = 0;      // posisi penulisan berikutnya pada buffer
int jumlah = 0;      // jumlah sampel yang sudah terisi

float suhuMin = 1000.0;
float suhuMax = -1000.0;

float hitungMovingAverage(float nilaiBaru) {
  buffer[indeks] = nilaiBaru;
  indeks = (indeks + 1) % N;
  if (jumlah < N) jumlah++;

  float total = 0;
  for (int i = 0; i < jumlah; i++) {
    total += buffer[i];
  }
  return total / jumlah;
}

// [TAMBAHAN] Menghitung simpangan baku sampel dari isi buffer.
// Parameter rata adalah rata-rata buffer (hasil moving average).
float hitungStdDev(float rata) {
  if (jumlah < 2) return 0.0;          // 1 sampel belum bisa dihitung variasinya
  float jumlahKuadrat = 0;
  for (int i = 0; i < jumlah; i++) {
    float selisih = buffer[i] - rata;  // jarak tiap data terhadap rata-rata
    jumlahKuadrat += selisih * selisih;
  }
  return sqrt(jumlahKuadrat / (jumlah - 1)); // dibagi (n-1) = simpangan baku sampel
}

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  float suhu = dht.readTemperature();

  if (isnan(suhu)) {
    Serial.println("Pembacaan gagal, data diabaikan");
  } else {
    float suhuFilter = hitungMovingAverage(suhu);
    float stdDev = hitungStdDev(suhuFilter);   // [TAMBAHAN] hitung simpangan baku buffer

    if (suhu < suhuMin) suhuMin = suhu;
    if (suhu > suhuMax) suhuMax = suhu;

    Serial.print("Mentah: ");
    Serial.print(suhu);
    Serial.print(" | Filter: ");
    Serial.print(suhuFilter);
    Serial.print(" | Std: ");                  // [TAMBAHAN] tampilkan simpangan baku
    Serial.print(stdDev);
    Serial.print(" | Min: ");
    Serial.print(suhuMin);
    Serial.print(" | Max: ");
    Serial.println(suhuMax);
  }

  delay(2000);
}
