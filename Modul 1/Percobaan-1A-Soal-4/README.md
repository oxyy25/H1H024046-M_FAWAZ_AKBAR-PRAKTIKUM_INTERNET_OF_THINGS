# Modifikasi Program Akuisisi DHT22 — Rata-rata 5 Kali Pembacaan

Program asli membaca sensor DHT22 sekali setiap 2 detik dan langsung menampilkan hasilnya. Modifikasi ini mengubah program agar mengambil 5 kali pembacaan terlebih dahulu, lalu menampilkan nilai rata-ratanya. 

## Penjelasan baris-baris yang ditambahkan/diubah dari kode sebelumnya

### 1. Konstanta jumlah sampling
```cpp
const int JUMLAH_SAMPLING = 5; // jumlah pembacaan yang akan dirata-ratakan
```
Baris ini berfungsi untuk menyimpan angka 5 sebagai patokan berapa kali sensor akan dibaca sebelum dirata-ratakan. Menggunakan `const` agar nilainya tidak bisa diubah secara tidak sengaja di tempat lain dalam program.

### 2. Variabel akumulator di awal `loop()`
```cpp
float totalSuhu = 0;       // akumulator penjumlahan suhu
float totalKelembaban = 0; // akumulator penjumlahan kelembaban
int dataValid = 0;         // penghitung berapa kali pembacaan berhasil (bukan NaN)
```
sebelum proses pembacaan dimulai. Ketiganya direset ke 0 setiap kali `loop()` dijalankan ulang, karena setiap siklus loop harus menghitung rata-rata yang baru dari 5 pembacaan yang baru pula:
- `totalSuhu` dan `totalKelembaban` akan menampung hasil penjumlahan dari beberapa kali pembacaan sensor.
- `dataValid` menghitung berapa banyak pembacaan yang benar-benar berhasil (tidak `NaN`), karena pembagi rata-rata harus memakai jumlah data yang valid, bukan selalu 5 (jika ada pembacaan yang gagal).

### 3. Perulangan `for` untuk mengambil 5 sampel
```cpp
for (int i = 0; i < JUMLAH_SAMPLING; i++) {
```
Baris ini untuk membungkus proses pembacaan sensor yang sebelumnya hanya dijalankan sekali per `loop()`. Sekarang proses pembacaan (`readHumidity`, `readTemperature`, pengecekan `isnan`, dan `delay`) diulang sebanyak `JUMLAH_SAMPLING` (5) kali di dalam satu siklus `loop()`.

### 4. Pembacaan sensor (dipindah ke dalam `for`)
```cpp
float kelembaban = dht.readHumidity();
float suhu = dht.readTemperature();
```
Baris ini di pindah ke dalam blok `for` sehingga dieksekusi berulang setiap iterasi.

### 5. Pengecekan validitas data (diubah isinya)
```cpp
if (isnan(kelembaban) || isnan(suhu)) {
  Serial.println("Gagal membaca data dari sensor DHT22!");
} else {
  totalSuhu += suhu;             // menjumlahkan suhu yang valid
  totalKelembaban += kelembaban; // menjumlahkan kelembaban yang valid
  dataValid++;                   // menambah jumlah pembacaan yang berhasil
}
```
- Baris `isnan(...)` dan pesan error tetap sama seperti program asli 
- Bagian `else` diubah: pada program asli bagian ini menampilkan nilai suhu/kelembaban langsung ke Serial Monitor. Pada versi modifikasi, bagian ini tidak lagi menampilkan hasil satu per satu, melainkan:
  - `totalSuhu += suhu;` menambahkan nilai suhu hasil pembacaan ke akumulator.
  - `totalKelembaban += kelembaban;` menambahkan nilai kelembaban ke akumulator.
  - `dataValid++;` menambah penghitung data valid sebanyak 1.

### 6. Delay di dalam loop `for`
```cpp
delay(2000); // jeda antar pembacaan sensor DHT22 (minimal 2 detik)
```
Baris ini dipindah dari akhir `loop()`  ke dalam blok `for`, karena jeda 2 detik kini harus terjadi *di antara setiap kali sampling data diambil, bukan hanya sekali di akhir siklus `loop()`. 

### 7. Perhitungan dan penampilan rata-rata
```cpp
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
```
Blok ini dijalankan setelah blok `for` (5 kali pembacaan) selesai:
- `if (dataValid > 0)` memastikan pembagian tidak dilakukan dengan angka 0 (menghindari error division by zero).
- `rataSuhu = totalSuhu / dataValid;` menghitung rata-rata suhu dengan membagi total suhu dengan jumlah data yang valid (bukan selalu 5, karena bisa saja sebagian pembacaan gagal).
- `rataKelembaban = totalKelembaban / dataValid;` menghitung rata-rata kelembaban dengan cara yang sama.
- Baris-baris `Serial.print(...)` menampilkan hasil rata-rata ke Serial Monitor, menggantikan tampilan per-pembacaan pada program asli.
- Blok `else` menangani kasus ekstrem di mana semua 5 kali pembacaan gagal, sehingga program menampilkan pesan informatif alih-alih mencoba membagi dengan 0.
