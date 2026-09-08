
## Dokumentasi Percobaan 1A
![Dokumentasi Percobaan 1A](\repo Praktikum IoT\Modul 1\Dokumentasi\Percobaan-1A.jpeg)

# Penjelasan Kode Akuisisi Data Sensor DHT11
 
Kode ini digunakan untuk membaca data suhu dan kelembaban dari sensor DHT11 menggunakan mikrokontroler Arduino, kemudian menampilkan hasilnya melalui Serial Monitor.
 
---
 
## 1. Include Library
 
```cpp
#include <DHT.h>
```
 
Baris ini menyertakan (include) library DHT ke dalam program. Library ini berisi kumpulan fungsi siap pakai untuk berkomunikasi dan mengambil data dari sensor DHT (DHT11/DHT22).
 
---
 
## 2. Definisi Pin dan Tipe Sensor
 
```cpp
#define DHTPIN 2
#define DHTTYPE DHT11
```
 
- `#define DHTPIN 2` — Menentukan bahwa pin digital 2 pada Arduino digunakan sebagai jalur data (data pin) yang terhubung ke sensor DHT11.
- `#define DHTTYPE DHT11` — Menentukan tipe sensor yang digunakan, yaitu DHT11. 
 
---
 
## 3. Membuat Objek Sensor
 
```cpp
DHT dht(DHTPIN, DHTTYPE);
```
 
Baris ini membuat sebuah objek bernama `dht` dari class `DHT`, dengan parameter pin data (`DHTPIN`) dan tipe sensor (`DHTTYPE`) yang sudah didefinisikan sebelumnya. Objek `dht` inilah yang nantinya digunakan untuk memanggil fungsi-fungsi pembacaan sensor seperti `readTemperature()` dan `readHumidity()`.
 
---
 
## 4. Fungsi `setup()`
 
```cpp
void setup()
{
    Serial.begin(115200);
    dht.begin();
    Serial.println("Memulai akuisisi data sensor DHT11...");
}
```
 
Fungsi `setup()` hanya dijalankan sekali saat ESP32 pertama kali dinyalakan atau direset. Fungsi ini digunakan untuk inisialisasi/konfigurasi awal.
 
---
 
## 5. Fungsi `loop()`
 
```cpp
void loop()
{
    float kelembaban = dht.readHumidity();
    float suhu = dht.readTemperature();
    ...
}
```
 
Fungsi `loop()` dijalankan berulang-ulang secara terus-menerus selama ESP32 menyala. Di sinilah proses pembacaan data sensor terjadi secara periodik.
 
### a. Membaca data sensor
 
```cpp
float kelembaban = dht.readHumidity();
float suhu = dht.readTemperature();
```
 
- `dht.readHumidity()` — Membaca nilai kelembaban udara (dalam persen, %) dari sensor DHT11.
- `dht.readTemperature()` — Membaca nilai suhu (dalam derajat Celcius, °C) dari sensor DHT11.

### b. Validasi data (pengecekan error)
 
```cpp
if (isnan(kelembaban) || isnan(suhu))
{
    Serial.println("Gagal membaca data dari sensor DHT11!");
}
```
 
`isnan()` (is not a number) digunakan untuk memeriksa apakah nilai yang dibaca valid atau tidak.
 
### c. Menampilkan hasil pembacaan
 
```cpp
else
{
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C, Kelembaban: ");
    Serial.print(kelembaban);
    Serial.println(" %");
}
```
 
Jika data valid (tidak `NaN`), program akan mencetak hasil pembacaan suhu dan kelembaban ke Serial Monitor dalam satu baris, dengan format:
 
```
Suhu: 27.00 °C, Kelembaban: 65.00 %
```
 
### d. Jeda waktu (delay)
 
```cpp
delay(2000);
```
 
Memberikan jeda selama 2000 milidetik (2 detik) sebelum `loop()` mengulang kembali dari awal. 
 
---
 