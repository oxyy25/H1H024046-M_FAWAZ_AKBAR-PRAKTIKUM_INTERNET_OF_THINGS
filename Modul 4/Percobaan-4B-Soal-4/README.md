# Modifikasi Program MQTT Publish-Subscribe Dua Arah (Percobaan 4B, Soal No. 4)

## Tujuan Modifikasi
Menambahkan aktuator kedua berupa buzzer aktif di GPIO 27 dengan topic MQTT
terpisah (`unsoed/tk245004/kelompokAnda/buzzer`), sehingga LED dan buzzer dapat
dikendalikan secara independen, serta menambahkan validasi key `perintah`
pada pesan JSON yang diterima melalui MQTT, menggunakan fungsi `strcmp()`,
`digitalWrite()`, `pinMode()`, dan `client.subscribe()`.

## Penjelasan Baris Kode yang Ditambahkan / Diubah

Semua baris baru ditandai komentar `TAMBAHAN` / `DIUBAH` pada file
`Percobaan-4B-Soal-4.ino`. Tidak ada perubahan pada `hubungkanWiFi()` dan
`loop()` (publish data suhu DHT22 setiap 5 detik ke `topicData`).

### 1. Definisi topic dan pin baru di variabel global

```cpp
const char* topicBuzzer = "unsoed/tk245004/kelompokAnda/buzzer";
const int buzzerPin = 27;
```

| Baris kode | Penjelasan |
|---|---|
| `const char* topicBuzzer = ".../buzzer";` | Mendefinisikan topic MQTT khusus untuk mengendalikan buzzer, terpisah dari `topicPerintah` yang dipakai LED.|
| `const int buzzerPin = 27;` | Menentukan GPIO 27 sebagai pin keluaran ke buzzer aktif (kaki positif buzzer ke GPIO 27, kaki negatif ke GND). |

### 2. Validasi dan pembacaan JSON di dalam fungsi `callback()`, setelah `deserializeJson()` berhasil

```cpp
if (doc["perintah"].isNull()) return;

const char* perintah = doc["perintah"];
bool nyala = (String(perintah) == "ON");
```

| Baris kode | Penjelasan |
|---|---|
| `if (doc["perintah"].isNull()) return;` | Memeriksa apakah key `"perintah"` ada di dalam JSON. Jika tidak ada, program langsung `return` agar tidak memproses perintah yang tidak valid / null. |
| `const char* perintah = doc["perintah"];` | Membaca nilai key `"perintah"` dari JSON, dengan cara kerja yang sama seperti pada kode asli. |
| `bool nyala = (String(perintah) == "ON");` | Menyimpan hasil pengecekan perintah sekali saja menjadi variabel boolean, dipakai bersama oleh kedua aktuator (LED dan buzzer) agar tidak mengulang perbandingan string. |

### 3. Pembedaan topic di dalam fungsi `callback()`, menggantikan kontrol LED tunggal

```cpp
if (strcmp(topic, topicPerintah) == 0) {
  digitalWrite(ledPin, nyala ? HIGH : LOW);
  Serial.print("[LED] Perintah diterima -> ");
  Serial.println(perintah);
} else if (strcmp(topic, topicBuzzer) == 0) {
  digitalWrite(buzzerPin, nyala ? HIGH : LOW);
  Serial.print("[BUZZER] Perintah diterima -> ");
  Serial.println(perintah);
}
```

| Baris kode | Penjelasan |
|---|---|
| `if (strcmp(topic, topicPerintah) == 0)` | Membandingkan nama topic pesan masuk dengan topic LED. Fungsi `strcmp` bernilai 0 jika kedua string sama, sehingga blok ini hanya berjalan untuk pesan dari `.../perintah`. |
| `digitalWrite(ledPin, nyala ? HIGH : LOW);` | Menyalakan LED bila perintah "ON" dan mematikannya bila selain itu. Perilaku LED sama seperti program asli, hanya kini dibungkus seleksi topic. |
| `else if (strcmp(topic, topicBuzzer) == 0)` | Jika pesan datang dari topic buzzer (`.../buzzer`), blok ini yang berjalan.Membedakan sumber pesan agar kedua aktuator saling independen. |
| `digitalWrite(buzzerPin, nyala ? HIGH : LOW);` | Menyalakan buzzer bila perintah "ON" dan mematikannya bila selain itu (misalnya "OFF"). |
| `Serial.print("[LED] ...")` / `"[BUZZER] ..."` | Label di Serial Monitor untuk membedakan aktuator yang menerima perintah. |

### 4. Subscribe tambahan di dalam fungsi `hubungkanMQTT()`

```cpp
client.subscribe(topicPerintah);
client.subscribe(topicBuzzer);
```

| Baris kode | Penjelasan |
|---|---|
| `client.subscribe(topicPerintah);` | Mendaftarkan ESP32 untuk menerima pesan kendali LED, sama seperti program asli. Diletakkan di `hubungkanMQTT()` agar langganan dipulihkan setiap reconnect. |
| `client.subscribe(topicBuzzer);` | Mendaftarkan ESP32 untuk menerima pesan dari topic buzzer. Baris ini wajib ada agar `callback()` bisa menerima pesan buzzer setelah reconnect ke broker. |

### 5. Inisialisasi pin di dalam fungsi `setup()`

```cpp
pinMode(ledPin, OUTPUT);
pinMode(buzzerPin, OUTPUT);
digitalWrite(buzzerPin, LOW);  // buzzer mati saat awal
```

| Baris kode | Penjelasan |
|---|---|
| `pinMode(buzzerPin, OUTPUT);` | Mengatur pin buzzer sebagai output agar bisa dikendalikan dengan `digitalWrite()`, sama fungsinya seperti `pinMode(ledPin, OUTPUT)` pada kode asli. |
| `digitalWrite(buzzerPin, LOW);` | Memastikan buzzer mati saat board pertama menyala. |

## Dampak pada Data yang Diterima
Setelah modifikasi, `callback()` tidak lagi hanya mendengarkan satu topic
`.../perintah`, tetapi dua topic yang saling independen:

```json
{"perintah": "ON"}
```

Pesan di atas bila dikirim ke topic `unsoed/tk245004/kelompokAnda/perintah`
akan menyalakan LED melalui `digitalWrite(ledPin, HIGH)` dan ditampilkan pada
Serial Monitor sebagai `[LED] Perintah diterima -> ON`, tanpa mengubah kondisi
buzzer. Sebaliknya bila pesan yang sama dikirim ke topic
`unsoed/tk245004/kelompokAnda/buzzer`, buzzer akan berbunyi melalui
`digitalWrite(buzzerPin, HIGH)` dan ditampilkan sebagai
`[BUZZER] Perintah diterima -> ON`, tanpa mengubah kondisi LED. Pesan
`{"perintah": "OFF"}` pada masing-masing topic akan mematikan aktuator yang
bersangkutan. Proses publish data suhu `{"suhu": 28.5}` ke `topicData` setiap
5 detik tetap sama seperti program asli.

