# Modul 5 - Percobaan 5B: Akuisisi, Analisis, dan Visualisasi Data

## 1. Fungsi blok try-except pada `on_message()`

Blok try mencoba mengubah pesan MQTT menjadi JSON (json.loads), mengambil kunci "suhu", dan mengubahnya menjadi float. Blok except (ValueError, KeyError) menangkap kesalahan yang mungkin terjadi : 
- ValueError: payload bukan JSON valid (JSONDecodeError adalah turunan ValueError), atau nilai suhu tidak dapat diubah ke angka (misalnya "nan" atau teks)
- KeyError: JSON valid tetapi tidak memiliki kunci "suhu"
Jika blok dihilangkan,pesan yang tidak valid memunculkan exception di dalam callback. Pada paho-mqtt, exception ini diteruskan ke loop_forever() sehingga program berhenti (crash). Akibatnya data yang sudah terkumpul tidak sempat disimpan ke CSV, dianalisis, dan divisualisasikan.

## 2. Arti z-score dan alasan batas |z| > 2

Z-score z = (x - rata-rata) / simpangan baku menyatakan seberapa jauh suatu data menyimpang dari rata-rata, dalam satuan simpangan baku. z = 0 berarti tepat di rata-rata, z = +2 berarti 2 simpangan baku di atas rata-rata, z = -2 berarti 2 simpangan baku di bawahnya.

Batas |z| > 2 dipakai karena, pada data yang berdistribusi mendekati normal, sekitar 95% data berada dalam rentang rata-rata ± 2 simpangan baku. Data di luar rentang itu hanya sekitar 5% dan tergolong jarang terjadi, sehingga layak dicurigai sebagai anomali.


## 3. Kelemahan z-score pada data dengan tren tajam

Kelemahan utama z-score pada data yang memiliki tren naik / turun tajam adalah rata-rata dan simpangan baku dihitung dari seluruh data sekaligus, sehingga keduanya tidak mewakili kondisi data di setiap titik waktu. Pada tren naik atau turun, rata-rata global berada di tengah rentang data, akibatnya data di awal dan akhir yang sebenarnya normal justru memiliki |z| besar dan salah ditandai sebagai anomali (false positive). Di sisi lain, tren membuat simpangan baku membesar, sehingga lonjakan nyata di sekitar nilai rata-rata tidak terdeteksi (false negative). Nilai ekstrim juga ikut menaikkan rata-rata dan simpangan baku (masking), sehingga anomali tampak lebih "normal". 

## 4. Modifikasi: moving average (jendela 5) sebelum z-score

Program lengkap ada di `Percobaan-5B-Soal-4.py`. Bagian yang ditambahkan ditandai `[TAMBAHAN]`.

```python
JENDELA_MA = 5
```
Konstanta ukuran jendela moving average agar mudah diubah-ubah.

```python
df_csv = pd.read_csv("data_suhu.csv")
```
Membaca ulang data dari file CSV, sesuai permintaan soal bahwa moving average diterapkan pada data hasil CSV.

```python
df_csv["suhu_ma"] = df_csv["suhu"].rolling(window=JENDELA_MA, min_periods=1).mean()
```
Menghitung rata-rata 5 data terakhir untuk setiap baris. `min_periods=1` membuat 4 data pertama (yang jendelanya belum penuh) tetap dihitung, seperti perilaku `hitungMovingAverage()` pada ESP32.

```python
rata2_ma = df_csv["suhu_ma"].mean()
std_ma = df_csv["suhu_ma"].std()
```
Rata-rata dan simpangan baku dihitung dari data yang sudah dihaluskan.

```python
if std_ma == 0:
    df_csv["zscore_ma"] = 0.0
else:
    df_csv["zscore_ma"] = (df_csv["suhu_ma"] - rata2_ma) / std_ma
```
Z-score data hasil moving average. Pengecekan `std_ma == 0` mencegah pembagian dengan nol bila semua data identik.

```python
df_csv["anomali_ma"] = df_csv["zscore_ma"].abs() > 2
```
Menandai data dengan |z| > 2 sebagai anomali, sama seperti pada metode awal.

```python
print(f"Tanpa moving average        : {df['anomali'].sum()} anomali")
print(f"Dengan moving average (N=...): {df_csv['anomali_ma'].sum()} anomali")
df_csv.to_csv("data_suhu_ma.csv", index=False)
```
Mencetak perbandingan jumlah anomali dan menyimpan hasilnya ke `data_suhu_ma.csv`.

```python
plt.plot(... df_csv["suhu"] ...)       # data mentah (transparan)
plt.plot(... df_csv["suhu_ma"] ...)    # data hasil moving average
plt.scatter(... anom_ma ...)           # titik anomali (merah)
plt.savefig("grafik_suhu_ma.png")
```
Grafik kedua untuk membandingkan data mentah, data hasil moving average, dan anomali yang terdeteksi.

## Analisis perbandingan dengan data mentah
 
Secara umum moving average meredam lonjakan sesaat, sehingga jumlah anomali cenderung berkurang dibanding data mentah. Lonjakan tunggal dibagi lima oleh filter, tetapi perubahan yang bertahan lama (misalnya sumber panas didekatkan beberapa saat) tetap terdeteksi. Kekurangannya, pergeseran data (lag) dapat membuat waktu terjadinya anomali tampak sedikit tertunda.