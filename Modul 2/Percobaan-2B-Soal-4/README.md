## Modifikasi Program: Mode AP+STA pada ESP32

Program ini merupakan modifikasi/gabungan dari program percobaan 2A (mode Station)
dan percobaan 2B (mode Access Point) pada Modul Praktikum 2, sehingga ESP32 dapat
terhubung ke jaringan WiFi rumah (Station) sekaligus menyediakan Access Point
sendiri (AP) secara bersamaan.

## Penjelasan Baris Kode

### 1. Deklarasi kredensial untuk dua peran sekaligus
```cpp
const char* sta_ssid     = "NAMA_WIFI_RUMAH_ANDA";
const char* sta_password = "PASSWORD_WIFI_RUMAH_ANDA";

const char* ap_ssid     = "ESP32_AccessPoint";
const char* ap_password = "12345678";
```
- `sta_ssid` dan `sta_password` adalah kredensial jaringan WiFi rumah/eksternal yang
  akan disambungi ESP32 (peran sebagai klien/Station).
- `ap_ssid` dan `ap_password` adalah kredensial hotspot yang akan dibuat sendiri
  oleh ESP32 (peran sebagai Access Point). 

### 2. Mengatur mode WiFi menjadi AP+STA
```cpp
WiFi.mode(WIFI_AP_STA);
```
- `WIFI_AP_STA` memberitahu modul WiFi ESP32 untuk mengaktifkan kedua fungsi
  sekaligus: menjadi client (STA) dan penyedia jaringan (AP) dalam waktu yang sama.

### 3. Mengaktifkan bagian Access Point
```cpp
WiFi.softAP(ap_ssid, ap_password);
IPAddress apIP = WiFi.softAPIP();
```
- `WiFi.softAP()` menyalakan hotspot ESP32 dengan SSID dan password yang telah
  ditentukan.
- `WiFi.softAPIP()` mengambil alamat IP dari Access Point yang dibuat (default `192.168.4.1`), yang
  nantinya digunakan oleh perangkat lain untuk mengakses ESP32 melalui jalur AP.

### 4. Memulai koneksi Station dengan batas waktu (timeout)
```cpp
WiFi.begin(sta_ssid, sta_password);
...
unsigned long startAttempt = millis();
while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
  delay(500);
  Serial.print(".");
}
```
- `WiFi.begin()` memulai proses koneksi ke jaringan WiFi rumah pada sisi Station.
- Diberi batas waktu 15 detik (15000 ms) agar program tidak macet
  (hang) tanpa batas apabila SSID/password STA salah atau router tidak
  tersedia.

### 5. Penanganan hasil koneksi Station
```cpp
if (WiFi.status() == WL_CONNECTED) {
  // tampilkan IP, MAC, RSSI STA, nyalakan LED
} else {
  Serial.println("STA gagal terhubung ke WiFi rumah (AP tetap aktif).");
  digitalWrite(ledPin, LOW);
}
```
- Jika STA berhasil connect, informasi IP/MAC/RSSI ditampilkan dan LED menyala
  sebagai indikator.
- Jika STA gagal connect dalam batas waktu, program tetap melanjutkan eksekusi
  (tidak berhenti), dan yang terpenting Access Point tetap berjalan karena
  kedua mode bersifat independen satu sama lain.

### 6. Pemantauan berkala di `loop()`
```cpp
if (WiFi.status() == WL_CONNECTED) {
  Serial.println("Status STA: Terhubung ke WiFi rumah");
  digitalWrite(ledPin, HIGH);
} else {
  Serial.println("Status STA: Terputus dari WiFi rumah");
  digitalWrite(ledPin, LOW);
}

int jumlahClient = WiFi.softAPgetStationNum();
Serial.print("Jumlah perangkat terhubung ke AP: ");
Serial.println(jumlahClient);
```
- Bagian pertama memantau status koneksi sisi STA setiap 5 detik, sama seperti
  pada program percobaan 2A.
- Bagian kedua memantau sisi AP dengan `WiFi.softAPgetStationNum()`, yaitu
  jumlah perangkat yang sedang terhubung ke hotspot ESP32.
- Kedua pemantauan ini berjalan berdampingan dalam satu `loop()` karena ESP32
  memang menjalankan dua peran jaringan secara paralel.