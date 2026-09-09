## Dokumentasi Percobaan 1A
![Dokumentasi Percobaan 1A](Dokumentasi/Percobaan1A.jpeg)

## Modifikasi Program: Auto-Reconnect WiFi pada ESP8266 (Mode Station)

Program ini merupakan modifikasi dari program dasar percobaan 2A (Konfigurasi Mode
Station) pada Modul Praktikum 2, dengan tambahan fitur agar ESP8266 dapat secara
otomatis mencoba menghubungkan ulang (reconnect) ke jaringan WiFi apabila koneksi
terputus.

## Penjelasan Baris Kode yang Ditambahkan

### 1. Variabel global untuk pengaturan waktu reconnect
```cpp
unsigned long previousReconnectMillis = 0;
const unsigned long reconnectInterval = 5000;
```
- `previousReconnectMillis` menyimpan waktu (dalam milidetik) kapan terakhir kali
  program mencoba melakukan reconnect.
- `reconnectInterval` menentukan jarak waktu minimal antar percobaan reconnect
  (5000 ms / 5 detik), agar `WiFi.reconnect()` tidak dipanggil terus-menerus
  setiap iterasi `loop()` yang dapat membebani modul WiFi.

### 2. Mengaktifkan fitur auto-reconnect bawaan library (di `setup()`)
```cpp
WiFi.setAutoReconnect(true);
WiFi.persistent(true);
```
- `WiFi.setAutoReconnect(true)` mengaktifkan mekanisme reconnect otomatis internal
  pada library WiFi ESP32, sehingga modul WiFi akan berusaha menyambung kembali
  sendiri ketika koneksi terputus.
- `WiFi.persistent(true)` membuat konfigurasi SSID dan password disimpan pada
  flash, sehingga tetap tersedia untuk proses reconnect meskipun terjadi restart
  singkat pada modul WiFi.

### 3. Timeout saat percobaan koneksi awal (di `setup()`)
```cpp
unsigned long startAttempt = millis();
while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
  delay(500);
  Serial.print(".");
}
```
- Ditambahkan batas waktu 15 detik (15000 ms) untuk mencoba koneksi awal.

### 4. Penanganan jika gagal konek di awal (di `setup()`)
```cpp
if (WiFi.status() == WL_CONNECTED) {
  ...
} else {
  Serial.println("Gagal terhubung dalam waktu yang ditentukan, akan dicoba lagi di loop().");
  digitalWrite(ledPin, LOW);
}
```
- Jika koneksi awal gagal dalam batas waktu timeout, program tidak berhenti,
  melainkan melanjutkan ke `loop()` di mana proses reconnect otomatis akan
  diusahakan secara berkala.

### 5. Logika reconnect otomatis di dalam `loop()`
```cpp
unsigned long currentMillis = millis();
if (currentMillis - previousReconnectMillis >= reconnectInterval) {
  previousReconnectMillis = currentMillis;
  Serial.println("Mencoba menghubungkan ulang ke WiFi...");
  WiFi.disconnect();
  WiFi.reconnect();
}
```
- Blok ini hanya dijalankan ketika `WiFi.status() != WL_CONNECTED` (di dalam
  cabang `else` pemeriksaan status).
- Menggunakan `millis()` (bukan `delay()` panjang) untuk mengecek apakah sudah
  waktunya mencoba reconnect lagi, sesuai `reconnectInterval` yang ditentukan.
- `WiFi.disconnect()` membersihkan status koneksi lama sebelum mencoba
  menyambung ulang, agar tidak terjadi konflik status internal.
- `WiFi.reconnect()` memerintahkan ESP32 untuk mencoba menyambung kembali ke
  jaringan WiFi menggunakan SSID/password yang sama seperti sebelumnya.