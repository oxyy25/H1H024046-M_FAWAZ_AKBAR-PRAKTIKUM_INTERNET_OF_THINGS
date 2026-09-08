
## Dokumentasi Percobaan 2A
![Dokumentasi Percobaan 2A](\repo Praktikum IoT\Modul 1\Dokumentasi\Percobaan-2A.jpeg)

# Penjelasan Kode Kontrol Aktuator (Relay) Berdasarkan Suhu DHT11
 
Kode ini membaca suhu dari sensor DHT11, lalu secara otomatis menyalakan atau mematikan aktuator/relay berdasarkan nilai ambang batas (threshold) suhu tertentu.
 
---
 
## 1. Include Library
 
```cpp
#include <DHT.h>
```
 
Menyertakan library DHT yang berisi fungsi-fungsi untuk membaca data dari sensor DHT11 sehingga tidak perlu menulis protokol komunikasi sensor dari awal.
 
---
 
## 2. Definisi Pin dan Tipe Sensor
 
```cpp
#define DHTPIN 2
#define DHTTYPE DHT11
#define RELAYPIN 16
```
 
- `#define DHTPIN 2` — Pin digital 2 digunakan sebagai jalur data sensor DHT11.
- `#define DHTTYPE DHT11` — Menentukan tipe sensor yang dipakai, yaitu DHT11.
- `#define RELAYPIN 16` — Pin digital 16 digunakan untuk mengontrol modul relay (yang biasanya terhubung ke aktuator seperti kipas, pompa, atau pemanas).
 
---
 
## 3. Membuat Objek Sensor dan Variabel Threshold
 
```cpp
DHT dht(DHTPIN, DHTTYPE);
const float suhuThreshold = 30.0;
```
 
- `DHT dht(DHTPIN, DHTTYPE);` — Membuat objek `dht` dari class `DHT` dengan pin dan tipe sensor yang sudah ditentukan. Objek ini digunakan untuk memanggil fungsi pembacaan sensor seperti `readTemperature()`.
- `const float suhuThreshold = 30.0;` — Mendefinisikan variabel konstan bertipe `float` bernama `suhuThreshold` dengan nilai 30.0°C. Kata kunci `const` berarti nilai ini tidak bisa diubah selama program berjalan. Variabel ini menjadi acuan/batas suhu untuk menentukan kapan aktuator harus menyala.
---
 
## 4. Fungsi `setup()`
 
```cpp
void setup()
{
    Serial.begin(115200);
    dht.begin();
    pinMode(RELAYPIN, OUTPUT);
    digitalWrite(RELAYPIN, LOW);
}
```
 
Dijalankan sekali saat ESP32 dinyalakan/direset, untuk inisialisasi awal dari DHT11 dan Relay
 
## 5. Fungsi `loop()`
 
```cpp
void loop()
{
    float suhu = dht.readTemperature();
    ...
}
```
 
Dijalankan berulang-ulang terus-menerus selama Arduino menyala. Di sinilah proses pembacaan suhu dan pengambilan keputusan kontrol relay terjadi.
 
### a. Membaca suhu
 
```cpp
float suhu = dht.readTemperature();
```
 
Membaca nilai suhu (°C) dari sensor DHT11 dan menyimpannya ke variabel `suhu` bertipe `float`.
 
### b. Validasi data
 
```cpp
if (isnan(suhu))
{
    Serial.println("Gagal membaca data sensor!");
}
```
 
`isnan()` (is not a number) mengecek apakah pembacaan sensor gagal/tidak valid. Jika sensor gagal membaca (misalnya karena gangguan koneksi), nilai `suhu` akan menjadi `NaN`, dan program mencetak pesan **"Gagal membaca data sensor!"** tanpa mengubah kondisi relay.
 
### c. Menampilkan suhu dan mengontrol relay
 
```cpp
else
{
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C -> ");
    if (suhu > suhuThreshold)
    {
        digitalWrite(RELAYPIN, HIGH);
        Serial.println("Aktuator: ON");
    }
    else
    {
        digitalWrite(RELAYPIN, LOW);
        Serial.println("Aktuator: OFF");
    }
}
```
 
Jika data suhu valid, program akan:
 
1. **Mencetak nilai suhu** ke Serial Monitor.
2. Membandingkan suhu dengan `suhuThreshold` (30.0°C):
   - Jika `suhu > suhuThreshold` (suhu lebih dari 30°C):
     - `digitalWrite(RELAYPIN, HIGH);` — Mengirim sinyal HIGH ke pin relay, sehingga relay/aktuator menyala (ON).
     - Mencetak "Aktuator: ON".
   - Jika `suhu <= suhuThreshold` (suhu 30°C atau kurang):
     - `digitalWrite(RELAYPIN, LOW);` — Mengirim sinyal LOW ke pin relay, sehingga relay/aktuator mati (OFF).
     - Mencetak "Aktuator: OFF".
 
---
 
