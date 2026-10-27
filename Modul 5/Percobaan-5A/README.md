# Percobaan: Pemfilteran Data Suhu dengan Moving Average (Mock DHT22)
 ![Dokumentasi Percobaan 5A](/Modul%205/Dokumentasi/Percobaan-5A.jpeg) 
## Tujuan
Memahami dan mengimplementasikan pemfilteran data sensor suhu menggunakan metode moving average berbasis circular buffer, serta pencatatan nilai suhu minimum dan maksimum. Sensor DHT22 disimulasikan dengan fungsi mock (data tiruan berisi noise dan anomali) sehingga percobaan dapat dijalankan tanpa perangkat keras sensor.

## Penjelasan Kode

```cpp
// #include <DHT.h>

// #define DHTPIN 4
// #define DHTTYPE DHT22

// DHT dht(DHTPIN, DHTTYPE);
```
Bagian konfigurasi sensor DHT22 yang dikomentari karena percobaan menggunakan data mock:
- `DHT.h` : Library untuk membaca sensor suhu dan kelembaban keluarga DHT (DHT11/DHT22).
- `DHTPIN` : Pin data sensor DHT (GPIO4).
- `DHTTYPE` : Tipe sensor yang dipakai (`DHT22`).
- `DHT dht(DHTPIN, DHTTYPE)` : Membuat objek sensor DHT dengan pin dan tipe yang ditentukan.

Hapus tanda komentar jika ingin menggunakan sensor asli.

```cpp
const int N = 5;
float buffer[N];
int indeks = 0;
int jumlah = 0;
```
Menyimpan konfigurasi dan variabel untuk moving average:
- `N` : Jumlah sampel (ukuran jendela) moving average, yaitu 5 pembacaan terakhir.
- `buffer[N]` : Array yang menyimpan `N` pembacaan terakhir.
- `indeks` : Posisi penulisan berikutnya pada `buffer`.
- `jumlah` : Banyaknya sampel yang sudah terisi pada `buffer` (maksimal `N`).

```cpp
float suhuMin = 1000.0;
float suhuMax = -1000.0;
```
Menyimpan nilai suhu terendah dan tertinggi yang pernah terbaca:
- `suhuMin` : Diinisialisasi dengan nilai sangat besar (1000) agar pembacaan pertama pasti lebih kecil dan langsung menggantikannya.
- `suhuMax` : Diinisialisasi dengan nilai sangat kecil (-1000) agar pembacaan pertama pasti lebih besar dan langsung menggantikannya.

### Fungsi `hitungMovingAverage()`

```cpp
float hitungMovingAverage(float nilaiBaru)
```
Fungsi yang menerima satu pembacaan baru dan mengembalikan rata-rata dari sampel terbaru:
- `nilaiBaru` : Pembacaan suhu terbaru yang akan dimasukkan ke dalam `buffer`.

```cpp
buffer[indeks] = nilaiBaru;
indeks = (indeks + 1) % N;
if (jumlah < N) jumlah++;
```
Menyimpan nilai baru menggunakan konsep circular buffer:
- `buffer[indeks] = nilaiBaru` : Menyimpan nilai baru pada posisi `indeks`.
- `indeks = (indeks + 1) % N` : Memajukan `indeks` satu langkah dan kembali ke 0 setelah mencapai `N` (operator modulo), sehingga data paling lama otomatis tertimpa oleh data baru.
- `if (jumlah < N) jumlah++` : Menambah jumlah sampel terisi hingga mencapai `N`. Hal ini membuat rata-rata tetap valid pada awal program saat `buffer` belum penuh.

```cpp
float total = 0;
for (int i = 0; i < jumlah; i++) {
  total += buffer[i];
}
return total / jumlah;
```
Menghitung rata-rata:
- Perulangan menjumlahkan seluruh isi `buffer` yang sudah terisi (sebanyak `jumlah` elemen) ke dalam `total`.
- `total / jumlah` : Total dibagi jumlah sampel valid, lalu dikembalikan sebagai hasil moving average.

### Fungsi `bacaSuhuMock()`

```cpp
static unsigned long jumlahBaca = 0;
jumlahBaca++;
```
Menghitung berapa kali fungsi dipanggil. Variabel `static` menjaga nilainya tetap tersimpan antar pemanggilan fungsi.

```cpp
float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);
```
Membuat suhu dasar yang berubah secara bertahap mengikuti gelombang sinus. Suhu berosilasi halus di sekitar 28 °C dengan amplitudo 2 °C (rentang 26–30 °C), meniru perubahan suhu lingkungan yang lambat.

```cpp
float noise = random(-5, 6) * 0.1;
float suhu = suhuDasar + noise;
```
Menambahkan noise kecil untuk mensimulasikan fluktuasi sensor:
- `random(-5, 6)` : Menghasilkan bilangan bulat acak dari -5 hingga 5.
- `* 0.1` : Mengubahnya menjadi noise antara -0,5 hingga +0,5 °C.

```cpp
if (jumlahBaca % 25 == 0) {
    suhu += 8.0;
}
return suhu;
```
Mensimulasikan anomali berupa lonjakan suhu:
- Setiap pembacaan kelipatan 25, suhu ditambah 8 °C.
- Nilai akhir dikembalikan sebagai pengganti hasil `dht.readTemperature()`.

### Fungsi `setup()`

```cpp
Serial.begin(115200);
// dht.begin();
```
- `Serial.begin(115200)` : Mengaktifkan komunikasi serial dengan baud rate 115200 agar hasil pembacaan dapat ditampilkan di Serial Monitor.
- `dht.begin()` : Inisialisasi sensor DHT, dikomentari karena belum digunakan pada mode mock.

### Fungsi `loop()`

```cpp
float suhu = bacaSuhuMock(); //dht.readTemperature();
```
Membaca suhu dari fungsi mock. Pada penggunaan sensor asli, ganti dengan `dht.readTemperature()`.

```cpp
if (isnan(suhu)) {
  Serial.println("Pembacaan gagal, data diabaikan");
} else {
```
Memeriksa validitas pembacaan:
- `isnan(suhu)` : Bernilai `true` jika pembacaan gagal (`NaN`). Program menampilkan pesan kesalahan dan mengabaikan data tersebut.
- Jika valid, program melanjutkan ke proses pemfilteran dan pencatatan.

```cpp
float suhuFilter = hitungMovingAverage(suhu);
```
Memasukkan suhu mentah ke fungsi moving average dan menyimpan hasil rata-ratanya sebagai suhu terfilter.

```cpp
if (suhu < suhuMin) suhuMin = suhu;
if (suhu > suhuMax) suhuMax = suhu;
```
Memperbarui suhu minimum dan maksimum:
- `suhuMin` diganti bila pembacaan lebih rendah dari minimum sebelumnya.
- `suhuMax` diganti bila pembacaan lebih tinggi dari maksimum sebelumnya.
- Perbandingan memakai suhu **mentah**, sehingga lonjakan anomali ikut tercatat.

```cpp
Serial.print("Mentah: ");
Serial.print(suhu);
Serial.print(" | Filter: ");
Serial.print(suhuFilter);
Serial.print(" | Min: ");
Serial.print(suhuMin);
Serial.print(" | Max: ");
Serial.println(suhuMax);
```
Menampilkan nilai suhu mentah, suhu terfilter, minimum, dan maksimum dalam satu baris di Serial Monitor, misalnya `Mentah: 28.30 | Filter: 28.30 | Min: 28.30 | Max: 28.30`.

```cpp
delay(2000);
```
Memberi jeda 2 detik antar pembacaan. Jeda ini juga sesuai dengan kebutuhan sensor DHT22 yang memerlukan waktu minimal sekitar 2 detik antar pembacaan.

## Ringkasan Alur Program
1. ESP/Arduino menginisialisasi Serial Monitor pada baud rate 115200 (inisialisasi sensor DHT dikomentari karena memakai data mock).
2. Di dalam `loop()`, program membaca suhu dari `bacaSuhuMock()` yang menghasilkan suhu dasar sinus, noise acak, dan lonjakan +8 °C setiap pembacaan ke-25.
3. Jika pembacaan bernilai `NaN`, data diabaikan dan pesan kesalahan ditampilkan.
4. Jika valid, suhu dimasukkan ke `hitungMovingAverage()` yang menyimpannya ke circular buffer dan menghitung rata-rata dari maksimal 5 sampel terakhir.
5. Suhu mentah dibandingkan dengan `suhuMin` dan `suhuMax` untuk memperbarui nilai terendah dan tertinggi.
6. Suhu mentah, suhu terfilter, minimum, dan maksimum ditampilkan di Serial Monitor.
7. Program menunggu 2 detik, lalu siklus diulang. Pada data hasil filter, lonjakan anomali terlihat teredam dibandingkan data mentah.