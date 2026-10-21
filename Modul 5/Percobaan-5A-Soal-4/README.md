# Modul 5 - Percobaan 5A: Pengolahan Data Sensor pada ESP32

Isi: jawaban pertanyaan praktikum 1-3 dan penjelasan modifikasi program no. 4 (file `modul5_pengolahan_data.ino`).

## 1. Flowchart fungsi `hitungMovingAverage()`

![flowchart](/Modul%205/Dokumentasi/flowchart.jpg)

## 2. Jelaskan fungsi dari operasi (indeks + 1) % N pada program tersebut!

Operasi ini membuat buffer bersifat melingkar (circular buffer). Setelah indeks mencapai N-1 (posisi terakhir), hasil (N-1+1) % N = 0 mengembalikan indeks ke posisi awal. Dengan begitu, data terbaru selalu menimpa data paling lama dan buffer hanya menyimpan N pembacaan terakhir tanpa perlu menggeser seluruh isi array. Tanpa %, indeks akan melewati batas array dan menyebabkan penulisan memori di luar buffer.


## 3. Apa dampaknya terhadap data hasil filter apabila nilai N diperbesar menjadi 20 atau diperkecil menjadi 2? 

Jika nilai N diperbesar ke 20, Data yang dihasilkan sangat halus karena noise akan terendam kuat, tetapi kecepatan respons menjadi lambat untuk sepenuhnya mengikuti perubahan nyata.Jika nilai N diperkecil ke 2,data akan kurang halus karena banyak noise yang masih bisa lolos,tetapi kecepatan respons sangat cepat,hampir sama dengan data mentah


## 4. Modifikasi: simpangan baku pada buffer

Program lengkap ada di `Percobaan-5A-Soal-B.ino`. Bagian yang ditambahkan ditandai komentar `[TAMBAHAN]`.

### a. Pustaka matematika
```cpp
#include <math.h>
```
Dibutuhkan untuk fungsi `sqrt()` (akar kuadrat).

### b. Fungsi `hitungStdDev(float rata)`
```cpp
float hitungStdDev(float rata) {
```
Mendeklarasikan fungsi yang menerima rata-rata buffer (hasil moving average) dan mengembalikan simpangan baku.

```cpp
  if (jumlah < 2) return 0.0;
```
Dengan satu sampel, simpangan baku sampel tidak terdefinisi (pembagi n-1 = 0), sehingga dikembalikan 0.

```cpp
  float jumlahKuadrat = 0;
  for (int i = 0; i < jumlah; i++) {
    float selisih = buffer[i] - rata;
    jumlahKuadrat += selisih * selisih;
  }
```
Untuk setiap data di buffer, dihitung selisihnya terhadap rata-rata lalu dikuadratkan dan dijumlahkan. Hanya `jumlah` elemen yang dipakai agar elemen buffer yang belum terisi tidak ikut dihitung.

```cpp
  return sqrt(jumlahKuadrat / (jumlah - 1));
}
```
Jumlah kuadrat dibagi (n-1) lalu diakarkan, menghasilkan simpangan baku sampel (konsisten dengan `std()` pandas pada Percobaan 5B).

### c. Pemanggilan di `loop()`
```cpp
float stdDev = hitungStdDev(suhuFilter);
```
Dipanggil setelah `hitungMovingAverage()` sehingga buffer sudah berisi data terbaru, dan `suhuFilter` dipakai sebagai rata-ratanya.

```cpp
Serial.print(" | Std: ");
Serial.print(stdDev);
```
Menampilkan simpangan baku di Serial Monitor. Nilai kecil berarti suhu stabil; nilai membesar saat sensor diberi gangguan (misalnya dihembus napas).