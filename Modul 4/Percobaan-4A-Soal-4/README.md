# Modifikasi Program MQTT Subscribe (Percobaan 4A, Soal No. 4)

## Tujuan Modifikasi
Menambahkan kontrol kecerahan LED menggunakan PWM (analog) menggantikan
`digitalWrite()` (digital ON/OFF), serta menambahkan validasi key `perintah`
dan field tambahan `intensitas` (0-255) pada pesan JSON yang diterima
melalui MQTT, menggunakan fungsi `ledcAttach()`, `ledcWrite()`, dan `constrain()`.

## Penjelasan Baris Kode yang Ditambahkan / Diubah

Semua baris baru ditandai komentar `TAMBAHAN` / `DIUBAH` pada file
`Percobaan-4A-Soal-4.ino`. Tidak ada perubahan pada `hubungkanWiFi()`,
`hubungkanMQTT()`, dan `loop()`.

### 1. Konfigurasi PWM di variabel global, sebelum `WiFiClient`

```cpp
const int pwmFreq = 5000;      // frekuensi PWM 5 kHz
const int pwmResolusi = 8;     // resolusi 8 bit -> nilai duty 0-255
```

| Baris kode | Penjelasan |
|---|---|
| `const int pwmFreq = 5000;` | Menentukan frekuensi sinyal PWM sebesar 5 kHz untuk pin LED.|
| `const int pwmResolusi = 8;` | Menentukan resolusi PWM 8 bit, sehingga nilai duty cycle berada pada rentang 0-255 (sama seperti `analogWrite` pada Arduino). |

### 2. Validasi dan pembacaan JSON di dalam fungsi `callback()`, setelah `deserializeJson()` berhasil

```cpp
if (doc["perintah"].isNull()) {
  Serial.println("Key 'perintah' tidak ditemukan");
  return;
}

const char* perintah = doc["perintah"];
int intensitas = doc["intensitas"] | 255;  // default 255 bila tidak dikirim
intensitas = constrain(intensitas, 0, 255); // batasi 0-255
```

| Baris kode | Penjelasan |
|---|---|
| `if (doc["perintah"].isNull())` | Memeriksa apakah key `"perintah"` ada di dalam JSON. Jika tidak ada (misalnya pesan hanya `{"intensitas":100}`), program menampilkan pesan peringatan dan `return` agar tidak memproses perintah yang tidak valid / null. |
| `int intensitas = doc["intensitas"] \| 255;` | Membaca field tambahan "intensitas" dari JSON dengan nilai default `255` (cahaya penuh) bila field tersebut tidak dikirim. Mekanisme ini menggunakan operator `\|` dari ArduinoJson, dengan cara kerja yang sama seperti `doc["perintah"]` pada kode asli. |
| `intensitas = constrain(intensitas, 0, 255);` | Membatasi nilai intensitas agar tetap dalam rentang duty PWM 0-255. Jika pengirim mengirim nilai di luar rentang (misalnya `-10` atau `500`), nilai otomatis ke batas terdekat. |

### 3. Kendali LED dengan PWM di dalam fungsi `callback()`, menggantikan `digitalWrite()`

```cpp
if (String(perintah) == "ON") {
  ledcWrite(ledPin, intensitas);  // PWM menggantikan digitalWrite HIGH
  Serial.print("Aktuator: ON, intensitas = ");
  Serial.println(intensitas);
} else if (String(perintah) == "OFF") {
  ledcWrite(ledPin, 0);  // duty 0 = LED mati
  Serial.println("Aktuator: OFF");
}
```

| Baris kode | Penjelasan |
|---|---|
| `ledcWrite(ledPin, intensitas);` | Menyalakan LED dengan kecerahan sesuai nilai `intensitas` (0 redup/mati - 255 paling terang). Baris ini menggantikan `digitalWrite(ledPin, HIGH)` pada program asli. |
| `ledcWrite(ledPin, 0);` | Mematikan LED dengan duty cycle 0 (tidak ada pulsa). Baris ini menggantikan `digitalWrite(ledPin, LOW)` pada program asli. Nilai intensitas yang dikirim ikut diabaikan saat perintah `OFF`. |

### 4. Inisialisasi PWM di dalam fungsi `setup()`, menggantikan `pinMode()`

```cpp
ledcAttach(ledPin, pwmFreq, pwmResolusi);
ledcWrite(ledPin, 0);  // LED mati saat awal
```

| Baris kode | Penjelasan |
|---|---|
| `ledcAttach(ledPin, pwmFreq, pwmResolusi);` | Menghubungkan `ledPin` (GPIO 26) ke channel PWM dengan frekuensi dan resolusi yang telah didefinisikan di atas. Baris ini menggantikan `pinMode(ledPin, OUTPUT)` pada program asli. |
| `ledcWrite(ledPin, 0);` | Memastikan LED mati saat board pertama menyala. |

## Dampak pada Data yang Diterima
Setelah modifikasi, `callback()` tidak lagi hanya memahami `{"perintah":"ON"}`
/ `{"perintah":"OFF"}`, tetapi juga field tambahan `intensitas`:

```json
{"perintah":"ON","intensitas":128}
```

Pesan di atas akan menghasilkan duty PWM 128 (kecerahan setengah) melalui
`ledcWrite(ledPin, intensitas)`, dan akan ikut ditampilkan pada Serial Monitor
sebagai `Aktuator: ON, intensitas = 128` sebagai bukti bahwa field intensitas
berhasil diterima. Jika field `intensitas` tidak dikirim, misalnya
`{"perintah":"ON"}`, LED menyala penuh (default 255).