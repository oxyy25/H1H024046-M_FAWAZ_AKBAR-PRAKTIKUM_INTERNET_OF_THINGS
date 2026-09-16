# Modifikasi Program HTTP POST (Percobaan 3A, Soal No. 4)

## Tujuan Modifikasi
Menambahkan data waktu (dalam milidetik sejak ESP32 dinyalakan) ke dalam data JSON
yang dikirim melalui HTTP POST, menggunakan fungsi `millis()`.

## Penjelasan Baris Kode yang Ditambahkan

Semua baris baru ditambahkan di dalam fungsi `loop()`, tepat setelah data `suhu`
dan `kelembaban` dimasukkan ke dalam objek JSON, dan sebelum data tersebut
di-serialize menjadi string.

```cpp
unsigned long waktuMillis = millis();   // mengambil waktu (ms) sejak board menyala
doc["waktu_ms"] = waktuMillis;          // menambahkan field "waktu_ms" ke JSON
```

| Baris kode | Penjelasan |
|---|---|
| `unsigned long waktuMillis = millis();` | Memanggil fungsi bawaan `millis()`, yang mengembalikan jumlah milidetik sejak ESP32 mulai menyala (atau sejak reset terakhir). Nilai ini disimpan dalam variabel `waktuMillis` bertipe `unsigned long` karena nilai `millis()` dapat menjadi sangat besar dan tidak boleh bernilai negatif. |
| `doc["waktu_ms"] = waktuMillis;` | Menambahkan pasangan key-value baru ke dalam objek JSON (`JsonDocument doc`), dengan key `"waktu_ms"` dan value berupa nilai waktu yang telah diambil sebelumnya. Baris ini menggunakan mekanisme yang sama seperti `doc["suhu"]` dan `doc["kelembaban"]` pada kode asli. |

## Dampak pada Data yang Dikirim
Setelah modifikasi, `requestBody` (hasil `serializeJson(doc, requestBody)`) akan
berbentuk seperti berikut, dengan satu field tambahan `waktu_ms`:

```json
{"suhu":28.5,"kelembaban":65.0,"waktu_ms":123456}
```

Data ini kemudian dikirim ke `httpbin.org/post` melalui `http.POST(requestBody)`
seperti pada program asli, dan akan ikut ditampilkan pada isi response (echo)
dari server sebagai bukti bahwa field waktu berhasil dikirim.

## Catatan
- `millis()` menghitung waktu sejak board dinyalakan/reset, bukan waktu nyata (real time/epoch).
  Jika dibutuhkan waktu nyata (misalnya tanggal dan jam), diperlukan tambahan library NTP (misalnya `NTPClient` atau fungsi `configTime()`).
- Tidak ada bagian lain dari program asli yang diubah; struktur `setup()`, koneksi WiFi,
  dan proses pengiriman HTTP POST tetap sama.