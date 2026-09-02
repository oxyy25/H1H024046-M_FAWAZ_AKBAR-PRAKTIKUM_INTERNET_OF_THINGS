# Modifikasi Program Kendali Aktuator — Histerisis Dua Ambang Batas

Program asli menggunakan satu ambang batas (`suhuThreshold = 30.0`) untuk menyalakan dan mematikan aktuator, yang berpotensi menyebabkan aktuator menyala-mati berulang kali (chattering) jika suhu berosilasi di sekitar 30°C. Modifikasi ini menerapkan histerisis: aktuator menyala di atas 30°C, dan baru mati jika suhu turun di bawah 28°C. Di antara 28°C dan 30°C, status aktuator dipertahankan seperti sebelumnya (tidak berubah).


## Penjelasan baris-baris yang ditambahkan/diubah

### 1. Dua konstanta ambang batas (menggantikan satu `suhuThreshold`)
```cpp
const float suhuNyala = 30.0; // ambang batas atas: aktuator menyala jika suhu melewati ini
const float suhuMati = 28.0;  // ambang batas bawah: aktuator mati jika suhu turun di bawah ini
```
Pada program asli hanya ada satu variabel `suhuThreshold`. Di sini diganti menjadi dua variabel:
- `suhuNyala` adalah batas atas  (begitu suhu melewati nilai ini, aktuator akan dinyalakan.)
- `suhuMati` adalah batas bawah  (aktuator baru dimatikan setelah suhu turun di bawah nilai ini.)

Selisih antara keduanya (30.0 − 28.0 = 2°C) adalah "zona netral" atau deadband yang membuat sistem tidak langsung bereaksi terhadap fluktuasi kecil.

### 2. Variabel status aktuator 
```cpp
bool statusAktuator = false; // menyimpan status aktuator saat ini (true = ON, false = OFF)
```
Baris ini sebagai variabel global bertipe `bool`. Pada program asli, status aktuator langsung ditentukan setiap kali `loop()` berjalan (berdasarkan satu kondisi `if`), sehingga tidak ada memori status sebelumnya. Pada versi histerisis, status aktuator harus diingat antar-iterasi `loop()`, karena ketika suhu berada di zona netral (28–30°C), aktuator harus tetap mempertahankan kondisi terakhirnya, bukan dihitung ulang dari nol. Nilai awal `false` berarti aktuator dianggap mati saat program pertama kali dijalankan (sesuai `digitalWrite(RELAYPIN, LOW)` di `setup()`).

### 3. Logika histerisis dengan `if / else if` 
```cpp
if (suhu > suhuNyala) {
  statusAktuator = true;  // suhu melewati batas atas, aktuator dinyalakan
} else if (suhu < suhuMati) {
  statusAktuator = false; // suhu turun di bawah batas bawah, aktuator dimatikan
}
// jika suhu berada di antara suhuMati dan suhuNyala, statusAktuator tidak diubah
```
Ini adalah inti dari mekanisme histerisis dan berbeda signifikan dari program asli yang belum dimodifikasi:
- Baris `if (suhu > suhuNyala)` mengubah `statusAktuator` menjadi `true` (ON) hanya jika suhu benar-benar melewati batas atas (30°C).
- Baris `else if (suhu < suhuMati)` mengubah `statusAktuator` menjadi `false` (OFF) hanya jika suhu benar-benar turun di bawah batas bawah (28°C).
- Jika suhu berada di antara 28°C dan 30°C, tidak ada cabang `if`/`else if` yang terpenuhi, sehingga `statusAktuator` tidak diubah, nilai sebelumnya tetap dipakai. Inilah yang mencegah aktuator menyala-mati berulang saat suhu berosilasi kecil di sekitar satu titik, karena kini ada jarak (deadband) antara titik nyala dan titik mati.

### 4. Menerapkan status ke pin relay (diubah)
```cpp
digitalWrite(RELAYPIN, statusAktuator ? HIGH : LOW); // menerapkan status ke pin relay
```
Pada program asli, `digitalWrite(RELAYPIN, HIGH/LOW)` dipanggil langsung di dalam masing-masing cabang `if`/`else`. Di versi histerisis, pemanggilan `digitalWrite` dipisahkan dan dilakukan satu kali setelah logika histerisis di atas, menggunakan nilai `statusAktuator` yang sudah ditentukan (baik itu hasil perubahan baru maupun nilai yang dipertahankan dari iterasi sebelumnya). Operator ternary `statusAktuator ? HIGH : LOW` berarti "jika `statusAktuator` bernilai `true`, kirim `HIGH`; jika tidak, kirim `LOW`".

### 5. Menampilkan status ke Serial Monitor (diubah)
```cpp
Serial.println(statusAktuator ? "Aktuator: ON" : "Aktuator: OFF");
```
Baris ini menggantikan dua `Serial.println` terpisah pada program asli (satu di cabang `if`, satu di cabang `else`). Kini cukup satu baris yang menampilkan teks berbeda tergantung nilai `statusAktuator`, konsisten dengan status aktuator yang sebenarnya diterapkan ke pin relay.

### 6. Delay dipindahkan ke luar blok `if/else` 
```cpp
delay(2000); // jeda pembacaan sensor DHT22 (minimal 2 detik)
```
Pada program asli, tidak ada `delay()` di akhir `loop()`. Pada versi ini, `delay(2000)` ditambahkan di akhir `loop()` (di luar blok `if (isnan(suhu))`) agar jeda antar pembacaan tetap terjadi baik ketika pembacaan berhasil maupun gagal, sesuai kebutuhan teknis sensor DHT22 yang memerlukan interval minimal 2 detik antar pembacaan.

