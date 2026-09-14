# Percobaan 2B: Konfigurasi Mode Access Point (AP)
![Dokumentasi Percobaan 2B](../Dokumentasi/Percobaan-2B.jpeg)

## Tujuan
Memahami dan mengimplementasikan konfigurasi ESP32 sebagai Access Point (AP)
yang dapat diakses langsung oleh perangkat lain (smartphone/laptop) tanpa
memerlukan router eksternal.

## Penjelasan Kode

```cpp
#include <ESP8266WiFi.h>
```
import library `ESP8266WiFi.h` bawaan ESP8266 yang menyediakan fungsi-fungsi untuk
mengatur koneksi jaringan, termasuk fungsi untuk membuat Access Point.

```cpp
const char* ap_ssid = "ESP32_AccessPoint";
const char* ap_password = "12345678"; // minimal 8 karakter
```
Menyimpan nama jaringan (SSID) dan password yang akan digunakan ESP8266 saat
memancarkan jaringan WiFi-nya sendiri. Password minimal 8 karakter sesuai
ketentuan standar keamanan WPA2.

### Fungsi `setup()`

```cpp
Serial.begin(115200);
```
Mengaktifkan komunikasi serial agar informasi status Access Point dapat
ditampilkan di Serial Monitor.

```cpp
WiFi.mode(WIFI_AP);
```
Mengatur mode WiFi ESP8266 menjadi Access Point, yaitu ESP8266 berperan
sebagai penyedia jaringan (hotspot) sendiri, bukan sebagai client yang
menyambung ke jaringan lain.

```cpp
WiFi.softAP(ap_ssid, ap_password);
```
Mengaktifkan Access Point dengan SSID dan password yang telah ditentukan.
Setelah baris ini dijalankan, ESP8266 mulai memancarkan sinyal WiFi yang dapat
ditemukan oleh perangkat lain di sekitarnya.

```cpp
IPAddress apIP = WiFi.softAPIP();
```
Mengambil alamat IP dari Access Point yang baru dibuat dan menyimpannya ke
variabel apIP. Nilainya secara default adalah `192.168.4.1`.

```cpp
Serial.println("Access Point aktif!");
Serial.print("SSID : ");
Serial.println(ap_ssid);
Serial.print("IP Address : ");
Serial.println(apIP);
```
Menampilkan informasi status Access Point ke Serial Monitor, yaitu pesan
konfirmasi bahwa AP telah aktif, nama SSID, dan alamat IP yang bisa digunakan
perangkat lain untuk mengakses ESP8266 (misalnya lewat web server).

### Fungsi `loop()`

```cpp
int jumlahClient = WiFi.softAPgetStationNum();
```
Mengambil jumlah perangkat (client) yang sedang terhubung ke Access Point
ESP32 saat ini, disimpan dalam variabel jumlahClient.

```cpp
Serial.print("Jumlah perangkat terhubung: ");
Serial.println(jumlahClient);
```
Menampilkan jumlah perangkat yang terhubung ke Serial Monitor, sehingga dapat
dipantau secara real-time berapa banyak perangkat yang sedang tersambung ke
Access Point.

```cpp
delay(5000);
```
Menjeda program selama 5 detik sebelum `loop()` mengulang kembali, sehingga
pengecekan jumlah client dilakukan secara berkala setiap 5 detik.

## Parameter Konfigurasi Access Point

| No | Parameter | Nilai |
|----|-----------|-------|
| 1 | SSID | ESP32_AccessPoint |
| 2 | Password | 12345678 |
| 3 | IP Address default | 192.168.4.1 |

## Ringkasan Alur Program
1. ESP32 diatur ke mode Access Point.
2. ESP32 memancarkan jaringan WiFi dengan SSID dan password yang ditentukan.
3. Alamat IP Access Point (`192.168.4.1`) ditampilkan di Serial Monitor.
4. Perangkat lain (HP/laptop) dapat menemukan dan menyambung ke jaringan
   tersebut menggunakan password yang telah ditentukan.
5. Jumlah perangkat yang terhubung dipantau dan ditampilkan setiap 5 detik.

