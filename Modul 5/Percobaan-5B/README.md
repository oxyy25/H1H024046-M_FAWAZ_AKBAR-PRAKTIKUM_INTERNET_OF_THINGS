# Percobaan: Akuisisi Data MQTT, Analisis Statistik, dan Deteksi Anomali dengan Python
 ![Dokumentasi Percobaan 5B](/Modul%205/Dokumentasi/Dokumentasi-Percobaan5B.png) 
## Tujuan
Memahami dan mengimplementasikan pengambilan data sensor suhu dari broker MQTT (broker.hivemq.com) menggunakan Python, menyimpannya ke file CSV, menganalisis statistik data, mendeteksi anomali dengan metode z-score, dan memvisualisasikan hasilnya dalam bentuk grafik.

## Penjelasan Kode

```python
import json
import time
import paho.mqtt.client as mqtt
import pandas as pd
import matplotlib.pyplot as plt
```
Library yang digunakan pada program:
- `json` : Library bawaan Python untuk mengubah string JSON menjadi objek Python (dictionary).
- `time` : Library bawaan Python yang digunakan untuk mengambil waktu saat data diterima.
- `paho.mqtt.client` : Library Paho MQTT yang menyediakan class `Client` untuk berkomunikasi dengan broker MQTT (connect, subscribe, dan menerima pesan). Program ini memakai Paho versi 2.x karena menggunakan `CallbackAPIVersion`.
- `pandas` : Library untuk mengolah data tabel (`DataFrame`), menghitung statistik, dan menyimpan ke CSV.
- `matplotlib.pyplot` : Library untuk membuat grafik.

```python
BROKER = "broker.hivemq.com"
PORT = 1883
TOPIC = "test"
JUMLAH_SAMPEL = 30
```
Menyimpan konfigurasi broker, topik, dan jumlah data:
- `BROKER` : Alamat broker MQTT. Pada percobaan ini menggunakan broker publik HiveMQ.
- `PORT` : Port broker MQTT. Nilai `1883` merupakan port standar MQTT tanpa enkripsi TLS.
- `TOPIC` : Topik MQTT yang di-subscribe untuk menerima data suhu. Topik ini harus sama dengan topik tempat ESP mem-publish data.
- `JUMLAH_SAMPEL` : Jumlah data yang dikumpulkan sebelum program berhenti menerima data (30 data).

```python
data = []
```
List kosong untuk menampung seluruh data yang diterima. Setiap elemen berupa dictionary berisi waktu dan suhu.

### Fungsi `on_connect()`

```python
def on_connect(client, userdata, flags, reason_code, properties):
    print("Terhubung ke broker, kode:", reason_code)
    client.subscribe(TOPIC)
```
Fungsi callback yang dipanggil otomatis setiap kali koneksi ke broker berhasil dibuat:
- `reason_code` : Kode hasil koneksi dari broker (`Success` bila berhasil).
- `client.subscribe(TOPIC)` : Melakukan subscribe ke topik data suhu agar pesan yang masuk dapat diterima. Subscribe dilakukan di dalam `on_connect()` agar otomatis diulang bila koneksi terputus lalu tersambung kembali.

### Fungsi `on_message()`

```python
def on_message(client, userdata, msg):
```
Fungsi callback yang dipanggil otomatis setiap ada pesan baru masuk pada topik yang di-subscribe. Parameter `msg` berisi topik dan payload pesan.

```python
try:
    payload = json.loads(msg.payload.decode())
    suhu = float(payload["suhu"])
except (ValueError, KeyError):
    print("Data tidak valid, diabaikan")
    return
```
Memproses payload pesan, contoh: `{"suhu":27}`:
- `msg.payload.decode()` : Mengubah payload byte menjadi string.
- `json.loads(...)` : Mengubah string JSON menjadi dictionary Python.
- `float(payload["suhu"])` : Mengambil nilai kunci `"suhu"` dan mengubahnya menjadi bilangan desimal.
- Jika format JSON salah (`ValueError`) atau kunci `"suhu"` tidak ada (`KeyError`), program menampilkan pesan dan langsung `return` sehingga data tidak valid diabaikan.

```python
data.append({"waktu": time.strftime("%Y-%m-%d %H:%M:%S"), "suhu": suhu})
print(f"[{len(data)}/{JUMLAH_SAMPEL}] suhu = {suhu}")
```
Menyimpan data valid ke list `data` beserta stempel waktu penerimaan (format `tahun-bulan-tanggal jam:menit:detik`), lalu menampilkan progres pengumpulan data di terminal.

```python
if len(data) >= JUMLAH_SAMPEL:
    client.disconnect()
```
Jika jumlah data sudah mencapai `JUMLAH_SAMPEL`, program memutus koneksi dari broker. Pemutusan ini membuat `loop_forever()` berhenti sehingga program lanjut ke tahap berikutnya.

### Tahap 1: Akuisisi Data dari MQTT

```python
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
```
- `mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)` : Membuat objek MQTT client dengan callback API versi 2 (sesuai bentuk parameter `on_connect` di atas).
- `client.on_connect` dan `client.on_message` : Mendaftarkan fungsi callback agar dipanggil saat koneksi berhasil dan saat pesan masuk.

```python
client.connect(BROKER, PORT)
client.loop_forever()
```
- `client.connect(BROKER, PORT)` : Menghubungkan client ke broker pada alamat dan port yang ditentukan.
- `client.loop_forever()` : Menjalankan loop jaringan MQTT secara terus-menerus dan memblokir program sampai `client.disconnect()` dipanggil. Selama itu, pesan masuk diteruskan ke `on_message()`.

### Tahap 2: Simpan ke CSV

```python
df = pd.DataFrame(data)
df.to_csv("data_suhu.csv", index=False)
print("Data tersimpan di data_suhu.csv")
```
- `pd.DataFrame(data)` : Mengubah list dictionary menjadi tabel dengan kolom `waktu` dan `suhu`.
- `df.to_csv("data_suhu.csv", index=False)` : Menyimpan tabel ke file `data_suhu.csv` tanpa kolom indeks.

### Tahap 3: Analisis Statistik dan Deteksi Anomali (Z-Score)

```python
rata2 = df["suhu"].mean()
std = df["suhu"].std()
```
Menghitung statistik dasar data suhu:
- `rata2` : Nilai rata-rata suhu.
- `std` : Standar deviasi suhu (pandas memakai standar deviasi sampel).

```python
if std == 0:
    df["zscore"] = 0.0
else:
    df["zscore"] = (df["suhu"] - rata2) / std
```
Menghitung z-score setiap data, yaitu seberapa jauh suatu nilai dari rata-rata dalam satuan standar deviasi: `z = (suhu - rata-rata) / std`. Jika `std` bernilai 0 (semua data sama), z-score diisi 0 untuk menghindari pembagian dengan nol.

```python
df["anomali"] = df["zscore"].abs() > 2
```
Menandai data sebagai anomali bila nilai mutlak z-score lebih dari 2, artinya data berada lebih dari 2 standar deviasi dari rata-rata. Hasilnya berupa kolom boolean (`True` untuk anomali).

```python
print(f"Rata-rata: {rata2:.2f}")
print(f"Std deviasi: {std:.2f}")
print(f"Minimum : {df['suhu'].min():.2f}")
print(f"Maksimum: {df['suhu'].max():.2f}")
print(f"Jumlah anomali: {df['anomali'].sum()}")
```
Menampilkan ringkasan statistik di terminal: rata-rata, standar deviasi, minimum, maksimum, dan jumlah anomali (dengan `sum()` menghitung banyaknya nilai `True`).

### Tahap 4: Visualisasi

```python
plt.figure(figsize=(10,4))
plt.plot(df.index, df["suhu"], marker="o", label="Suhu")
```
Membuat grafik berukuran 10x4 inci dan menggambar garis suhu terhadap urutan data, dengan penanda lingkaran pada setiap titik.

```python
anom = df[df["anomali"]]
plt.scatter(anom.index, anom["suhu"], color="red", s=100, label="Anomali", zorder=3)
```
Memfilter baris yang anomali, lalu menggambarnya sebagai titik merah berukuran besar. `zorder=3` membuat titik anomali tampil di atas garis suhu.

```python
plt.axhline(rata2, color="gray", linestyle="--", label="Rata-rata")
```
Menambahkan garis horizontal putus-putus berwarna abu-abu pada nilai rata-rata sebagai acuan.

```python
plt.xlabel("Urutan data")
plt.ylabel("Suhu (°C)")
plt.title("Data Suhu dan Deteksi Anomali")
plt.legend()
plt.tight_layout()
```
Mengatur label sumbu, judul, legenda, dan merapikan tata letak grafik.

```python
plt.savefig("grafik_suhu.png")
plt.show()
```
- `plt.savefig("grafik_suhu.png")` : Menyimpan grafik ke file `grafik_suhu.png`.
- `plt.show()` : Menampilkan grafik di jendela. Pemanggilan `savefig()` dilakukan sebelum `show()` agar gambar yang tersimpan tidak kosong.

## Cara Menjalankan
1. Instal library: `pip install paho-mqtt pandas matplotlib` (pastikan `paho-mqtt` versi 2.x).
2. Pastikan perangkat ESP mem-publish data JSON berformat `{"suhu":27}` ke topik yang sama dengan `TOPIC`.
3. Jalankan program: `python nama_file.py`.
4. Tunggu hingga 30 data terkumpul. Hasilnya berupa `data_suhu.csv`, ringkasan statistik di terminal, dan `grafik_suhu.png`.

## Catatan
- Topik pada program ini adalah `test`, sedangkan pada kode ESP8266 Percobaan 4B data dipublish ke `test/data`. Samakan keduanya (misalnya ubah `TOPIC` menjadi `"test/data"`) agar data dapat diterima.
- Program ini memakai port `1883` (tanpa TLS), sedangkan ESP8266 pada Percobaan 4B memakai port `8883` (TLS). Keduanya tetap dapat berkomunikasi karena sama-sama terhubung ke broker yang sama, hanya jalur koneksinya berbeda.
- Broker publik HiveMQ bisa dipakai siapa saja, jadi gunakan nama topik yang cukup unik agar data tidak tercampur dengan pengguna lain.
- Jika tidak ada data valid yang diterima, `DataFrame` akan kosong dan tahap analisis akan gagal. Program hanya sampai tahap itu bila koneksi diputus sebelum data terkumpul.
- Metode z-score dengan ambang 2 bersifat sensitif terhadap jumlah data. Pada 30 sampel dengan data acak, beberapa titik bisa saja terdeteksi sebagai anomali meskipun bukan lonjakan nyata.

## Ringkasan Alur Program
1. Program membuat MQTT client, mendaftarkan callback `on_connect()` dan `on_message()`, lalu terhubung ke `broker.hivemq.com` pada port 1883.
2. Saat terhubung, `on_connect()` melakukan subscribe ke topik data suhu.
3. `loop_forever()` menjaga koneksi aktif. Setiap pesan masuk diteruskan ke `on_message()` yang mem-parsing JSON, mengambil nilai `suhu`, dan menyimpannya beserta stempel waktu ke list `data`. Pesan tidak valid diabaikan.
4. Setelah 30 data terkumpul, koneksi diputus sehingga `loop_forever()` berhenti.
5. Data diubah menjadi `DataFrame` dan disimpan ke `data_suhu.csv`.
6. Program menghitung rata-rata dan standar deviasi, lalu z-score setiap data. Data dengan `|z| > 2` ditandai sebagai anomali.
7. Ringkasan statistik (rata-rata, std, min, maks, jumlah anomali) ditampilkan di terminal.
8. Grafik suhu dibuat dengan titik anomali berwarna merah dan garis rata-rata, disimpan ke `grafik_suhu.png`, lalu ditampilkan.