# Sistem Auto Tartil & Jam Panel Musholla Berbasis ESP32

Sistem cerdas berbasis ESP32 yang dirancang khusus untuk mengelola jadwal sholat, pemutaran audio tarhim/tartil sebelum adzan, dan adzan secara otomatis. Sistem ini dilengkapi dengan antarmuka Web Server lokal (Access Point) untuk memudahkan konfigurasi tanpa memerlukan koneksi internet aktif secara terus-menerus.

Sistem ini dioptimalkan untuk menyala 24/7 dengan manajemen RAM yang efisien (menghindari memory leak), perlindungan EEPROM dan Hardware Watchdog Timer untuk keandalan maksimal di lapangan.
## 🌟 Fitur Utama

* **Jadwal Otomatis Non-Blocking:** Mengeksekusi pemutaran audio tartil dan adzan sesuai jadwal sholat secara presisi tanpa menghentikan proses sistem lainnya.
* **Konfigurasi via Web Server:** Pengaturan jadwal, volume, durasi audio, dan parameter lainnya dapat dilakukan melalui *browser* HP/Laptop menggunakan jaringan WiFi Access Point bawaan alat.
* **Manajemen Memori Tangguh:** Menggunakan C-string dan `snprintf` untuk mencegah *Heap Fragmentation*, serta sistem penulisan EEPROM berbasis *Dirty Flag* untuk memperpanjang umur Flash Memory.
* **Hardware Watchdog Timer (HWDT):** Fitur pemulihan otomatis yang akan me-*restart* mikrokontroler jika terdeteksi *hang* atau gangguan sistem.
* **Kontrol Amplifier Otomatis:** Mengendalikan *Relay* untuk menghidupkan dan mematikan amplifier (Toa) hanya saat audio sedang diputar, sehingga menghemat daya dan mengurangi *noise*.

## 🛠️ Kebutuhan Perangkat Keras (Hardware)

* 1x ESP32 Development Board
* 1x Modul DFRobot DFPlayer Mini
* 1x MicroSD Card (maks. 32GB, format FAT32)
* 1x Modul Relay 1-Channel (Active Low)
* 3x LED (sebagai indikator status)
* Kabel Jumper secukupnya

## 📌 Konfigurasi Pin (Pinout)

Sistem ini menggunakan pemetaan pin berikut pada ESP32:

| Komponen / Fungsi | Pin ESP32 | Keterangan |
| :--- | :--- | :--- |
| **DFPlayer RX** | `Pin 16` | Hubungkan ke TX DFPlayer (Gunakan Resistor 1K) |
| **DFPlayer TX** | `Pin 17` | Hubungkan ke RX DFPlayer |
| **Relay Amplifier** | `Pin 26` | Kontrol daya Amplifier (Active Low) |
| **LED Run** | `Pin 13` | Indikator program berjalan (Efek *Breathing*) |
| **LED Status Normal** | `Pin 14` | Indikator sistem komunikasi aktif |
| **LED WiFi** | `Pin 27` | Indikator status jaringan |

## 🚀 Cara Instalasi & Penggunaan

### 1. Persiapan MicroSD (Audio)
Susun *file* audio MP3 di dalam MicroSD. Karena sistem menggunakan metode `playFolder`, pastikan nama folder menggunakan format angka (misalnya `01`, `02`) dan nama *file* diawali dengan angka berurutan berformat 3 digit (misalnya `001.mp3`, `002.mp3`).

### 2. Konfigurasi Jaringan Bawaan
Setelah perangkat dinyalakan, ESP32 akan memancarkan sinyal WiFi (Access Point) dengan kredensial bawaan:
* **SSID:** `JAM_PANEL`
* **Password:** `00000000` (atau sesuai yang tersimpan di EEPROM)

### 3. Mengakses Web Panel
Gunakan *browser* di HP atau PC yang terhubung ke WiFi alat, lalu akses alamat IP bawaan:
`http://192.168.2.1/setPanel`

### 4. Daftar API (*Endpoint*) Konfigurasi Utama
Sistem ini menerima parameter *query string* untuk berbagai pengaturan (misalnya `http://192.168.2.1/setPanel?VOL=20`). Beberapa *endpoint* penting meliputi:
* `TIME:` : Menyesuaikan waktu RTC lokal.
* `VOL:` : Mengatur volume DFPlayer (0-30).
* `HR:` : Mengatur jadwal aktif harian dan *playlist* tartil.
* `ADZAN:` / `NAMAFILE:` : Mengatur durasi spesifik untuk *file* audio adzan dan tartil.
* `PLAY:` / `STOP` : Mengontrol pemutaran audio secara manual untuk pengujian.

## 👨‍💻 Catatan Pengembang
Kode ini telah dioptimalkan untuk menghindari penggunaan tipe data `String` konvensional pada rutinitas berulang guna memastikan stabilitas jangka panjang. Fungsi `delay()` telah dieliminasi dan digantikan dengan algoritma pengecekan `millis()` berbasis *state-machine*.