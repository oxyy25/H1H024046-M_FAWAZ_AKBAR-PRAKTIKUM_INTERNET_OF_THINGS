import json
import time
import paho.mqtt.client as mqtt
import pandas as pd
import matplotlib.pyplot as plt

BROKER = "broker.hivemq.com"
PORT = 1883
TOPIC = "unsoed/tk245004/kelompokAnda/data"  # = topicData ESP32
JUMLAH_SAMPEL = 30   # jumlah data
JENDELA_MA = 5       # [TAMBAHAN] ukuran jendela moving average

data = []

def on_connect(client, userdata, flags, reason_code, properties):
    print("Terhubung ke broker, kode:", reason_code)
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        suhu = float(payload["suhu"])
    except (ValueError, KeyError):
        print("Data tidak valid, diabaikan")
        return

    data.append({"waktu": time.strftime("%Y-%m-%d %H:%M:%S"), "suhu": suhu})
    print(f"[{len(data)}/{JUMLAH_SAMPEL}] suhu = {suhu}")

    if len(data) >= JUMLAH_SAMPEL:
        client.disconnect()

# --- Tahap 1: akuisisi data dari MQTT ---
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT)
client.loop_forever()

# --- Tahap 2: simpan ke CSV ---
df = pd.DataFrame(data)
df.to_csv("data_suhu.csv", index=False)
print("Data tersimpan di data_suhu.csv")

# --- Tahap 3: analisis statistik dan deteksi anomali (z-score) ---
rata2 = df["suhu"].mean()
std = df["suhu"].std()

if std == 0:
    df["zscore"] = 0.0
else:
    df["zscore"] = (df["suhu"] - rata2) / std
df["anomali"] = df["zscore"].abs() > 2

print(f"Rata-rata : {rata2:.2f}")
print(f"Std deviasi: {std:.2f}")
print(f"Minimum   : {df['suhu'].min():.2f}")
print(f"Maksimum  : {df['suhu'].max():.2f}")
print(f"Jumlah anomali: {df['anomali'].sum()}")

# --- [TAMBAHAN] Tahap 3b: moving average pada data CSV, lalu z-score ---
# Baca ulang data dari file CSV (sesuai soal: "pada data hasil CSV")
df_csv = pd.read_csv("data_suhu.csv")

# Rata-rata bergerak jendela 5 data; min_periods=1 agar 4 data awal tetap dihitung
df_csv["suhu_ma"] = df_csv["suhu"].rolling(window=JENDELA_MA, min_periods=1).mean()

# Statistik dari data yang sudah dihaluskan
rata2_ma = df_csv["suhu_ma"].mean()
std_ma = df_csv["suhu_ma"].std()

# Z-score dihitung dari data hasil moving average (hindari pembagian dengan 0)
if std_ma == 0:
    df_csv["zscore_ma"] = 0.0
else:
    df_csv["zscore_ma"] = (df_csv["suhu_ma"] - rata2_ma) / std_ma
df_csv["anomali_ma"] = df_csv["zscore_ma"].abs() > 2

# Perbandingan jumlah anomali sebelum dan sesudah moving average
print("\n=== Perbandingan deteksi anomali ===")
print(f"Tanpa moving average        : {df['anomali'].sum()} anomali")
print(f"Dengan moving average (N={JENDELA_MA}): {df_csv['anomali_ma'].sum()} anomali")
df_csv.to_csv("data_suhu_ma.csv", index=False)

# --- Tahap 4: visualisasi ---
plt.figure(figsize=(10, 4))
plt.plot(df.index, df["suhu"], marker="o", label="Suhu")
anom = df[df["anomali"]]
plt.scatter(anom.index, anom["suhu"], color="red", s=100, label="Anomali", zorder=3)
plt.axhline(rata2, color="gray", linestyle="--", label="Rata-rata")
plt.xlabel("Urutan data")
plt.ylabel("Suhu (°C)")
plt.title("Data Suhu dan Deteksi Anomali")
plt.legend()
plt.tight_layout()
plt.savefig("grafik_suhu.png")
plt.show()

# [TAMBAHAN] Grafik perbandingan data mentah vs moving average
plt.figure(figsize=(10, 4))
plt.plot(df_csv.index, df_csv["suhu"], marker="o", alpha=0.5, label="Mentah")
plt.plot(df_csv.index, df_csv["suhu_ma"], linewidth=2, label=f"Moving average (N={JENDELA_MA})")
anom_ma = df_csv[df_csv["anomali_ma"]]
plt.scatter(anom_ma.index, anom_ma["suhu_ma"], color="red", s=100, label="Anomali (MA)", zorder=3)
plt.xlabel("Urutan data")
plt.ylabel("Suhu (°C)")
plt.title("Perbandingan Data Mentah dan Moving Average")
plt.legend()
plt.tight_layout()
plt.savefig("grafik_suhu_ma.png")
plt.show()
