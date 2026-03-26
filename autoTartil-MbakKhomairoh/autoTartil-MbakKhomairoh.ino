/*
 * Controller menggunakan ESP32 
 * Controller difungsikan untuk menghitung dan memproses waktu pemutaran Auto Tartil
 * Controller difungsikan juga untuk Acces Point (komunikasi ke User)
*/
 
#include <DFRobotDFPlayerMini.h>
#include <EEPROM.h>
#include <TimeLib.h>
#include <esp_task_wdt.h>

// Atur batas waktu WDT (misalnya 15 detik)
constexpr uint8_t WDT_TIMEOUT = 15;

#define PASSWORD_LEN 20   // maksimal 15 karakter + '\0'
//KONFIGURASI WIFI
char ssid[PASSWORD_LEN]     = "JAM_PANEL";
char password[PASSWORD_LEN] = "00000000";

//LIBRARY UNTUK ACCES POINT
#include <WiFi.h>
#include <WebServer.h>

//OBJEK dfPlayer
#define dfSerial Serial2
DFRobotDFPlayerMini dfplayer;

//OBJEK WEB SERVER
WebServer server(80);

IPAddress local_IP(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);

#define RELAY_PIN         26
#define RUN_LED           13
#define NORMAL_STATUS_LED 14
#define LED_WIFI          27

#define EEPROM_SIZE 1000

#define HARI_TOTAL  8 // 7 hari + SemuaHari (index ke-7)
#define WAKTU_TOTAL 5
#define MAX_FILE    50
#define MAX_FOLDER  2 //3
#define JEDA_ANTAR_TARTIL 20 //500 jeda antar file tartil dalam milidetik

//#define DEBUG 1

struct WaktuConfig {
  byte aktif;
  byte aktifAdzan;
  byte fileAdzan;
  byte tartilDulu;
  byte folder;
  byte list[5];
};

WaktuConfig jadwal[HARI_TOTAL][WAKTU_TOTAL];
uint8_t durasiAdzan[MAX_FILE];
uint16_t durasiTartil[MAX_FOLDER][MAX_FILE];

byte volumeDFPlayer;

uint8_t jamSholat[WAKTU_TOTAL]; //= {4, 12, 15, 18, 19};
uint8_t menitSholat[WAKTU_TOTAL];// = {30, 0, 30, 0, 30};

bool tartilSedangDiputar = false;
uint32_t tartilMulaiMillis = 0;
byte tartilFolder = 0;
byte tartilIndex = 0;

uint16_t tartilCounter = 0;
uint16_t targetDurasi = 0;
uint32_t lastTick = 0;

bool jedaAktif = false;
uint32_t jedaMulaiMillis = 0;

WaktuConfig *currentCfg = nullptr;

uint32_t lastTriggerMillis = 0;
bool sudahEksekusi = false;
bool adzanSedangDiputar = false;
uint32_t adzanMulaiMillis = 0;
uint16_t adzanDurasi = 0;

uint32_t lastAdzanTick = 0;
uint16_t adzanCounter = 0;
uint16_t targetDurasiAdzan = 0;

byte currentDay = 0;

// Tambahan untuk relay delay dan manual
// uint32_t relayOffDelayMillis = 0;
// bool relayMenungguMati = false;
bool manualSedangDiputar = false;
bool adzanManualSedangDiputar = false;

//variabel untuk led status system
static uint8_t m_Counter = 0;
constexpr uint16_t waveStepDelay = 20;  // Delay antar frame LED breathing (ms)
static uint32_t lastWaveMillis = 0;

bool lastNormalStatus = false;
uint32_t lastTimeReceived = 0;
constexpr uint32_t TIMEOUT_INTERVAL = 50000; // 70 detik, lebih dari 1 menit

bool wsConnected = false;
bool wifiConnected = false;
unsigned long lastWiFiAttempt = 0;
constexpr uint32_t wifiRetryInterval = 5000;

bool autoTartilEnable = true;
bool voiceClock = true;

void getData(const char* input) {
  Serial.println(input);
}


void handleSetTime() {
  //String data = "";
  char dataBuffer[250];
  
   if (server.hasArg("Tm")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Tm=%s", server.arg("Tm").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Settingan jam berhasil diupdate");
    return;
  }
  if (server.hasArg("text")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "text=%s", server.arg("text").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Settingan text berhasil diupdate");
    return;
  }
  if (server.hasArg("name")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "name=%s", server.arg("name").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Settingan nama berhasil diupdate");
    return;
  }
  if (server.hasArg("Br")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Br=%s", server.arg("Br").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecerahan berhasil diupdate");
    return;
  }
  if (server.hasArg("Spdt")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Spdt=%s", server.arg("Spdt").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecepatan kalender berhasil diupdate");
    return;
  }
  if (server.hasArg("Sptx1")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Sptx1=%s", server.arg("Sptx1").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecepatan info 1 berhasil diupdate");
    return;
  }
  if (server.hasArg("Sptx2")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Sptx2=%s", server.arg("Sptx2").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecepatan info 2 berhasil diupdate");
    return;
  }
  if (server.hasArg("Sptx3")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Sptx3=%s", server.arg("Sptx3").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecepatan info 2 berhasil diupdate");
    return;
  }
  if (server.hasArg("Sptx4")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Sptx4=%s", server.arg("Sptx4").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecepatan info 2 berhasil diupdate");
    return;
  }
  if (server.hasArg("Sptx5")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Sptx5=%s", server.arg("Sptx5").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecepatan info 2 berhasil diupdate");
    return;
  }
  if (server.hasArg("Spnm")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Spnm=%s", server.arg("Spnm").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Kecepatan nama berhasil diupdate");
    return;
  }
  if (server.hasArg("Iq")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Iq=%s", server.arg("Iq").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"iqomah diupdate");
    return;
  }
  if (server.hasArg("Dy")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Dy=%s", server.arg("Dy").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"displayBlink diupdate");
    return;
  }
  if (server.hasArg("Kr")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Kr=%s", server.arg("Kr").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"Selisih jadwal sholat diupdate");
    return;
  }
  if (server.hasArg("Lt")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Lt=%s", server.arg("Lt").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"latitude diupdate");
    return;
  }
  if (server.hasArg("Lo")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Lo=%s", server.arg("Lo").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"longitude diupdate");
    return;
  }
  if (server.hasArg("Tz")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Tz=%s", server.arg("Tz").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"timezone diupdate");
    return;
  }
  if (server.hasArg("Al")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Al=%s", server.arg("Al").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"altitude diupdate");
    return;
  }
  if (server.hasArg("Da")) { 
    snprintf(dataBuffer, sizeof(dataBuffer), "Da=%s", server.arg("Da").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");// "durasi adzan diupdate");
    return;
  }
  if (server.hasArg("CoHi")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "CoHi=%s", server.arg("CoHi").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain", "OK");//"coreksi hijriah diupdate");
    return;
  }

  if (server.hasArg("Bzr")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Bzr=%s", server.arg("Bzr").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain","OK");// (stateBuzzer) ? "Suara Diaktifkan" : "Suara Dimatikan");
    return;
  }
  if (server.hasArg("bzrClk")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "bzrClk=%s", server.arg("bzrClk").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain","OK");// (stateBuzzer) ? "Suara Diaktifkan" : "Suara Dimatikan");
    return;
  }
  if (server.hasArg("alarm")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "alarm=%s", server.arg("alarm").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain","OK");// (stateBuzzer) ? "Suara Diaktifkan" : "Suara Dimatikan");
    return;
  }
  if (server.hasArg("alarmOn")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "alarmOn=%s", server.arg("alarmOn").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain","OK");// (stateBuzzer) ? "Suara Diaktifkan" : "Suara Dimatikan");
    return;
  }
  if (server.hasArg("alarmOff")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "alarmOff=%s", server.arg("alarmOff").c_str());
    getData(dataBuffer);
    server.send(200, "text/plain","OK");// (stateBuzzer) ? "Suara Diaktifkan" : "Suara Dimatikan");
    return;
  }
  if (server.hasArg("mode")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "mode=%s", server.arg("mode").c_str());
    getData(dataBuffer);
    parseData(dataBuffer);
    server.send(200, "text/plain","OK");// (stateBuzzer) ? "Suara Diaktifkan" : "Suara Dimatikan");
    return;
  }
  // --- PLAY ---
  if (server.hasArg("PLAY")) {
    uint8_t folder = 0, file = 0;
    // sscanf langsung mengambil dua angka yang dipisah koma
    sscanf(server.arg("PLAY").c_str(), "%d,%d", &folder, &file);
    
    snprintf(dataBuffer, sizeof(dataBuffer), "PLAY:%d,%d", folder, file);
    parseData(dataBuffer); 
    server.send(200, "text/plain", "OK");
    return;
  }

  // --- PLAD ---
  if (server.hasArg("PLAD")) {
    uint8_t file = 0;
    sscanf(server.arg("PLAD").c_str(), "%d", &file);
    
    snprintf(dataBuffer, sizeof(dataBuffer), "PLAD:%d", file);
    parseData(dataBuffer);
    server.send(200, "text/plain", "OK");
    return;
  }

  // --- NAMAFILE ---
  if (server.hasArg("NAMAFILE")) {
    uint8_t folder = 0, file = 0;
    uint16_t durasi = 0;
    sscanf(server.arg("NAMAFILE").c_str(), "%d,%d,%d", &folder, &file, &durasi);
    
    snprintf(dataBuffer, sizeof(dataBuffer), "NAMAFILE:%d,%d,%d", folder, file, durasi);
    parseData(dataBuffer);
    server.send(200, "text/plain", "OK");
    return;
  }

  // --- ADZAN ---
  if (server.hasArg("ADZAN")) {
    uint8_t file = 0;
    uint16_t durasi = 0;
    sscanf(server.arg("ADZAN").c_str(), "%d,%d", &file, &durasi);
    
    snprintf(dataBuffer, sizeof(dataBuffer), "ADZAN:%d,%d", file, durasi);
    parseData(dataBuffer);
    server.send(200, "text/plain", "OK");
    return;
  }

  // --- At ---
  if (server.hasArg("At")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "At:%s", server.arg("At").c_str());
    parseData(dataBuffer);
    server.send(200, "text/plain", "OK");
    return;
  }

  // --- Vc ---
  if (server.hasArg("Vc")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "Vc:%s", server.arg("Vc").c_str());
    parseData(dataBuffer);
    server.send(200, "text/plain", "OK");
    return;
  }
  
  if (server.hasArg("status")) {
    snprintf(dataBuffer, sizeof(dataBuffer), "%s","status=1");
    getData(dataBuffer);
    server.send(200, "text/plain", "CONNECTED");
     return;
  }

  if (server.hasArg("newPassword")) {
      snprintf(dataBuffer, sizeof(dataBuffer), "newPassword=%s", server.arg("newPassword").c_str());
      parseData(dataBuffer);
      server.send(200, "text/plain", "OK");
      return;
  }
}

void AP_init() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);

  server.on("/setPanel", handleSetTime);
  server.begin();
 
}

void setup() {
  EEPROM.begin(EEPROM_SIZE);//
  
  digitalWrite(RELAY_PIN, HIGH); // Awal mati
  pinMode(RUN_LED, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_WIFI, OUTPUT);
  pinMode(NORMAL_STATUS_LED, OUTPUT);

 // --- Inisialisasi Watchdog Timer untuk ESP32 Core v3.x ---
  esp_task_wdt_config_t wdt_config = {
    .timeout_ms = WDT_TIMEOUT * 1000,                // Ubah satuan detik menjadi milidetik
    .idle_core_mask = (1 << portNUM_PROCESSORS) - 1, // Memantau aktivitas di semua core
    .trigger_panic = true                            // Paksa restart jika mikrokontroler hang
  };
  
  esp_task_wdt_init(&wdt_config);
  esp_task_wdt_add(NULL); // Daftarkan fungsi loop() ke dalam pengawasan anjing penjaga
  // ---------------------------------------------------------
  Serial.println(F("Watchdog Timer Aktif!"));
  
  delay(1000);
  Serial.begin(9600);
  dfSerial.begin(9600, SERIAL_8N1, /*rx =*/16, /*tx =*/17);
 
  if (!dfplayer.begin(dfSerial,/*isACK = */true, /*doReset = */true)) {
    Serial.println("DFPlayer tidak terdeteksi!");
    while (1);
  }
  
  dfplayer.enableDAC(); // Pakai output DAC (line out)
  Serial.println("Sistem Auto Tartil Siap.");
  
  loadFromEEPROM();
  
  delay(1000);
  AP_init();
  dfplayer.volume(volumeDFPlayer);  
  digitalWrite(RELAY_PIN, HIGH); // Awal mati
}

void loop() {
  if (sudahEksekusi && millis() - lastTriggerMillis > 60000) {
    sudahEksekusi = false;
  }
  esp_task_wdt_reset();
  server.handleClient();
  cekDanPutarSholatNonBlocking();
  cekSelesaiTartil();
  cekSelesaiAdzan();
  cekSelesaiAdzanManual();
  cekSelesaiManual();
  getStatusRun();
 
}


/*/================= parsing data dari Akses Point =========================//
int getIntPart(String &s, int &pos) {
  int comma = s.indexOf(',', pos);
  if (comma == -1) comma = s.length();
  int val = s.substring(pos, comma).toInt();
  pos = comma + 1;
  return val;
}*/

void bacaDataSerial() {
  // Booking memori statis sebesar 512 byte (sesuaikan jika data lebih panjang)
  static char buffer[50]; 
  static uint16_t index = 0;

  while (Serial.available() > 0) {
    char c = Serial.read();

    // FILTER 1: Hanya terima karakter teks yang valid (ASCII 32 sampai 126)
    // Karakter sampah/noise (seperti '⸮') akan otomatis diabaikan.
    if (c >= 32 && c <= 126) {
      if (index < sizeof(buffer) - 1) { // Pelindung dari Buffer Overflow
        buffer[index++] = c;
      }
    }
    // FILTER 2: Tanda pesan selesai (Newline '\n' atau Carriage Return '\r')
    else if (c == '\n' || c == '\r') {
      if (index > 0) {
        buffer[index] = '\0'; // Kunci teks dengan null-terminator

        // --- DEBUG PINTU MASUK ---
        // Serial.print(F("\n>> Memanggil parseData() dengan Teks Bersih: '"));
        // Serial.print(buffer);
        // Serial.println(F("'"));
        // -------------------------

        parseData(buffer); // Kirim teks ke fungsi pembongkar data
        index = 0;         // Reset index ke 0 untuk siap menerima pesan baru
      }
    }
  }
}


// 1. Ubah parameter dari String menjadi const char*
void parseData(const char* data) {
  Serial.print(F("data=")); Serial.println(data);
  lastTimeReceived = millis();

  // --- Parsing TIME: ---

  const char* eq_ptr = strchr(data, '='); // Cari posisi '='
  
  // --- Parsing TIME: ---
  if (strncmp(data, "TIME:", 5) == 0) {
    const char* ptr = data + 5;
    
    uint8_t jam   = atoi(ptr);
    while (*ptr && *ptr >= '0' && *ptr <= '9') ptr++; if (*ptr) ptr++; 
    
    uint8_t menit = atoi(ptr);
    while (*ptr && *ptr >= '0' && *ptr <= '9') ptr++; if (*ptr) ptr++;
    
    uint8_t detik = atoi(ptr);
    while (*ptr && *ptr >= '0' && *ptr <= '9') ptr++; if (*ptr) ptr++;
    
    uint8_t hari  = atoi(ptr);

    Serial.print(F("[DEBUG TIME] Terekstrak -> Jam: ")); Serial.print(jam);
    Serial.print(F(", Menit: ")); Serial.print(menit);
    Serial.print(F(", Detik: ")); Serial.print(detik);
    Serial.print(F(", Hari: ")); Serial.println(hari);

    if (jam < 24 && menit < 60 && detik < 60 && hari < 7) {
     // Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(), jam, menit, detik));
      setTime(jam, menit, detik, 23, 4, 2026);
      currentDay = hari;
      Serial.println(F("[DEBUG TIME] RTC Berhasil Diupdate!"));
    } else {
      Serial.println(F("[ERROR TIME] Data waktu tidak masuk akal (Out of range)!"));
    }
    return;
  }

  // --- Parsing VOL: ---
  else if (strncmp(data, "VOL:", 4) == 0) {
    volumeDFPlayer = atoi(data + 4);

    Serial.print(F("[DEBUG VOL] Volume diubah ke: ")); 
    Serial.println(volumeDFPlayer);
    
    dfplayer.volume(volumeDFPlayer);
    saveToEEPROM();
    return;
  }

  // --- Parsing HR: --- (Jadwal harian)
  // Format lama: HR:hari|W0:aktif,aktifAdzan,fileAdzan,tartilDulu,folder-list0-list1-list2-list3-list4
  else if (strncmp(data, "HR:", 3) == 0) {
    const char* ptr = data + 3;
    
    int hari = atoi(ptr);
    if (hari < 0 || hari >= HARI_TOTAL) return;

    for (int w = 0; w < WAKTU_TOTAL; w++) {
      char tag[8];
      snprintf(tag, sizeof(tag), "|W%d:", w);
      
      const char* w_ptr = strstr(data, tag);
      if (w_ptr == nullptr) continue;

      w_ptr += strlen(tag); // Lompat ke nilai setelah tag
      WaktuConfig &cfg = jadwal[hari][w];

      Serial.print(F("  -> Waktu [")); Serial.print(w);
      Serial.print(F("] Aktif:")); Serial.print(cfg.aktif);
      Serial.print(F(", Adzan:")); Serial.print(cfg.aktifAdzan);
      Serial.print(F(", FileAdzan:")); Serial.print(cfg.fileAdzan);
      Serial.print(F(", Tartil:")); Serial.print(cfg.tartilDulu);
      Serial.print(F(", Folder:")); Serial.print(cfg.folder);
      Serial.print(F(", ListFile: "));

      // Ambil 5 nilai berurutan (asumsi dipisah koma atau karakter non-angka)
      cfg.aktif      = atoi(w_ptr); while (*w_ptr && *w_ptr >= '0' && *w_ptr <= '9') w_ptr++; if (*w_ptr) w_ptr++;
      cfg.aktifAdzan = atoi(w_ptr); while (*w_ptr && *w_ptr >= '0' && *w_ptr <= '9') w_ptr++; if (*w_ptr) w_ptr++;
      cfg.fileAdzan  = atoi(w_ptr); while (*w_ptr && *w_ptr >= '0' && *w_ptr <= '9') w_ptr++; if (*w_ptr) w_ptr++;
      cfg.tartilDulu = atoi(w_ptr); while (*w_ptr && *w_ptr >= '0' && *w_ptr <= '9') w_ptr++; if (*w_ptr) w_ptr++;
      cfg.folder     = atoi(w_ptr); while (*w_ptr && *w_ptr >= '0' && *w_ptr <= '9') w_ptr++; if (*w_ptr) w_ptr++;

      // Ambil list file (dipisah dengan dash '-')
      for (int i = 0; i < 5; i++) {
        cfg.list[i] = atoi(w_ptr);

        Serial.print(cfg.list[i]); Serial.print(F("-"));
        
        const char* dash = strchr(w_ptr, '-');
        if (dash) {
          w_ptr = dash + 1;
        } else {
          break; // Keluar dari loop jika tidak ada dash lagi
        }
        Serial.println(); // Enter setelah selesai 1 waktu
      }
    }
    saveToEEPROM();
    return;
  }

  // --- Parsing PLAY: ---
  else if (strncmp(data, "PLAY:", 5) == 0) {
    const char* ptr = data + 5;
    byte folder = atoi(ptr);
    while (*ptr && *ptr >= '0' && *ptr <= '9') ptr++; if (*ptr) ptr++;
    byte file   = atoi(ptr);

    Serial.print(F("[DEBUG PLAY] Folder: ")); Serial.print(folder);
    Serial.print(F(", File: ")); Serial.print(file);

    if (folder >= 1 && folder < 12 && file >= 1 && file < MAX_FILE) {
      uint16_t durasi = durasiTartil[folder - 1][file];
      if (durasi > 0) {
        dfplayer.volume(volumeDFPlayer);
        dfplayer.playFolder(folder,file);
        digitalWrite(RELAY_PIN, LOW); // Relay NYALA
        tartilCounter       = 0;
        targetDurasi        = durasi;
        lastTick            = millis();
        manualSedangDiputar = true;
      }
    }
    return;
  }

  // --- Parsing PLAD: ---
  else if (strncmp(data, "PLAD:", 5) == 0) {
    byte file = atoi(data + 5);
    uint16_t durasi = durasiAdzan[file];

    Serial.print(F("[DEBUG PLAD] Play Adzan Manual File: ")); Serial.print(file);
    Serial.print(F(", Durasi Target: ")); Serial.println(durasi);
    
    if (durasi > 0) {
      dfplayer.volume(volumeDFPlayer);
      dfplayer.playFolder(2,file);
      digitalWrite(RELAY_PIN, LOW); // Relay NYALA
      adzanCounter             = 0;
      targetDurasiAdzan        = durasi;
      lastAdzanTick            = millis();
      adzanManualSedangDiputar = true;
    }
    return;
  }

  // --- Parsing STOP ---
  else if (strncmp(data, "STOP", 4) == 0) {
    dfplayer.stop();
    digitalWrite(RELAY_PIN, HIGH); // Relay MATI
    tartilSedangDiputar = false;
    adzanSedangDiputar  = false;
    manualSedangDiputar = false;
    Serial.println(F("[DEBUG STOP] DFPlayer & Relay telah DIMATIKAN secara paksa."));
    return;
  }

  // --- Parsing NAMAFILE: ---
  else if (strncmp(data, "NAMAFILE:", 9) == 0) {
    const char* ptr = data + 9;
    byte folder = atoi(ptr);
    while (*ptr && *ptr >= '0' && *ptr <= '9') ptr++; if (*ptr) ptr++;
    byte list   = atoi(ptr);
    while (*ptr && *ptr >= '0' && *ptr <= '9') ptr++; if (*ptr) ptr++;
    int durasi  = atoi(ptr);

    Serial.print(F("[DEBUG NAMAFILE] Durasi Tartil Disimpan -> Folder: ")); Serial.print(folder);
    Serial.print(F(", List: ")); Serial.print(list);
    Serial.print(F(", Durasi: ")); Serial.println(durasi);

    if (folder < MAX_FOLDER && list < MAX_FILE) {
      durasiTartil[folder][list] = durasi;
      saveToEEPROM();
    }
    return;
  }

  // --- Parsing ADZAN: ---
  else if (strncmp(data, "ADZAN:", 6) == 0) {
    const char* ptr = data + 6;
    byte file  = atoi(ptr);
    while (*ptr && *ptr >= '0' && *ptr <= '9') ptr++; if (*ptr) ptr++;
    int durasi = atoi(ptr);

    Serial.print(F("[DEBUG ADZAN] Durasi Adzan Disimpan -> File: ")); Serial.print(file);
    Serial.print(F(", Durasi: ")); Serial.println(durasi);

    if (file < MAX_FILE) {
      durasiAdzan[file] = durasi;
      saveToEEPROM();
    }
    return;
  }

  // --- Parsing JWS: ---
  /*/ Format asumi: JWS:jam1,menit1|jam2,menit2|jam3,menit3...
  else if (strncmp(data, "JWS:", 4) == 0) {
    const char* ptr = data + 4;
    for (int i = 0; i < WAKTU_TOTAL; i++) {
      jamSholat[i] = atoi(ptr);
      ptr = strchr(ptr, ',');
      if (!ptr) break;
      ptr++; // Lewati koma
      
      menitSholat[i] = atoi(ptr);

      Serial.print(jamSholat[i]); Serial.print(F(":")); 
      Serial.print(menitSholat[i]); Serial.print(F(" | "));
      
      ptr = strchr(ptr, '|');
      if (!ptr) break; // Jika tidak ada pemisah lagi, selesai
      ptr++; // Lewati pipa
    }
    saveToEEPROM();
  }*/
  // --- Parsing JWS: ---
  // Format asumi: JWS:jam1,menit1|jam2,menit2|jam3,menit3...
  else if (strncmp(data, "JWS:", 4) == 0) {
    const char* ptr = data + 4;
    bool adaPerubahan = false; // Flag penanda apakah ada data yang berubah

    for (int i = 0; i < WAKTU_TOTAL; i++) {
      uint8_t tempJam = atoi(ptr); // Tampung jam di variabel sementara

      ptr = strchr(ptr, ',');
      if (!ptr) break;
      ptr++; // Lewati koma
      
      uint8_t tempMenit = atoi(ptr); // Tampung menit di variabel sementara

      // Cek apakah data baru BERBEDA dengan data yang sedang berjalan di sistem
      if (jamSholat[i] != tempJam || menitSholat[i] != tempMenit) {
        jamSholat[i] = tempJam;     // Update array utama karena beda
        menitSholat[i] = tempMenit; // Update array utama karena beda
        adaPerubahan = true;        // Tandai bahwa ada perubahan data
      }

      Serial.print(tempJam); Serial.print(F(":")); 
      Serial.print(tempMenit); Serial.print(F(" | "));
      
      ptr = strchr(ptr, '|');
      if (!ptr) break; // Jika tidak ada pemisah lagi, selesai
      ptr++; // Lewati pipa
    }

    Serial.println(); // Enter untuk merapikan Serial Monitor

    // Eksekusi simpan HANYA jika terdeteksi ada perubahan angka
    if (adaPerubahan) {
      Serial.println(F("[INFO] Jadwal sholat diubah, menyimpan ke EEPROM..."));
      saveToEEPROM();
    } else {
      Serial.println(F("[INFO] Jadwal sholat sama persis, abaikan EEPROM (Hemat Flash)."));
    }
  }

  // --- Parsing At: --- (Auto Tartil)
  else if (strncmp(data, "At:", 3) == 0) {
    autoTartilEnable = atoi(data + 3);

    Serial.print(F("[DEBUG At] Status Auto Tartil diubah menjadi: ")); 
    Serial.println(autoTartilEnable ? F("AKTIF") : F("NONAKTIF"));
    saveToEEPROM();
    return;
  }

  // --- Parsing newPassword= ---
  else if (strncmp(data, "newPassword:", 12) == 0) {
    const char* pwd = data + 12;

    Serial.print(F("[DEBUG PASSWORD] Request ubah password ke: '"));
    Serial.print(pwd);
    Serial.println(F("'"));
    
    if (strlen(pwd) == 8) {
      strncpy(password, pwd, 9); // Copy 8 karakter + null terminator
      saveToEEPROM();
      delay(1000);
      ESP.restart();
    } else {
      Serial.println(F("Password invalid (harus 8 karakter)"));
    }
    return;
  }

  // Pemanggilan default jika ada data yang lolos tidak di-return sebelumnya
  // saveToEEPROM(); (Aktifkan jika memang setiap data yang tak teridentifikasi harus memicu simpan)
}
//============================== END =================================//
