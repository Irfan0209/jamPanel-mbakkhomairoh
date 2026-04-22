
//=================== new variabel ========================//
char * const pasar[]  = {"WAGE", "KLIWON", "LEGI", "PAHING", "PON"}; 
char * const Hari[]  = {"MINGGU","SENIN","SELASA","RABU","KAMIS","JUM'AT","SABTU"};
//const char * const bulanMasehi[] PROGMEM = {"JANUARI", "FEBRUARI", "MARET", "APRIL", "MEI", "JUNI", "JULI", "AGUSTUS", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DESEMBER" };
char* jadwal[] = {"IMSAK","SUBUH", "TERBT", "DUHUR", "ASHAR", "MAGRB", "ISYA'"};
char* jadwalAzzan[] = {"SUBUH","DZUHUR", "ASHAR", "MAGRIB", "ISYA'"};
char * namaBulanHijriah[] = {
    "MUHARRAM", "SHAFAR", "RABIUL AWAL",
    "RABIUL AKHIR", "JUMADIL AWAL", 
    "JUMADIL AKHIR", "RAJAB",
    "SYA'BAN", "RAMADHAN", "SYAWAL",
    "DZULQA'DAH", "DZULHIJAH"
};

uint16_t sholatSec[5];   // waktu sholat (detik)
//===================== convert jam & menit ke detik =============================//
inline uint32_t toSecond(uint8_t h, uint8_t m, uint8_t s = 0)
{
  return (uint32_t)h * 3600UL + (uint32_t)m * 60UL + s;
}

/*/================= animasi jadwal sholat new =================//
void updateAnimSholat() {
  if(adzan) return;
  
  RtcDateTime now = Rtc.GetDateTime();
  static int8_t y = 0, y1 = 0;
  static uint8_t s = 0, s1 = 0;
  static bool run = false;

  static uint32_t lsRn_y1 = 0;
  static uint32_t lsRn_y = 0;
  static uint32_t tHold = 0;
  static uint8_t list = 0;

  uint32_t Tmr = millis();

  // Pilih waktu sholat sesuai list
  float stime;
  switch (list) {
    case 0: stime = JWS.floatImsak; break;
    case 1: stime = JWS.floatSubuh; break;
    case 2: stime = JWS.floatTerbit; break;
    case 3: stime = JWS.floatDzuhur; break;
    case 4: stime = JWS.floatAshar; break;
    case 5: stime = JWS.floatMaghrib; break;
    case 6: stime = JWS.floatIsya; break;
    default: stime = 0; break;
  }

  // Transisi vertikal y1 (jam muncul/hilang)
  if ((Tmr - lsRn_y1) > 55) {
    lsRn_y1 = Tmr;

    if (s1 == 0 && y1 < 17) {
      y1++;
    } else if (s1 == 1 && y1 > 0) {
      y1--;
    }
  }
  //Serial.println("y1:" + String(y1));
  // Saat y1 selesai muncul, mulai animasi jadwal
  if (y1 == 17 && s1 == 0) {
    run = true; 
    if(now.Second() % 2 ){
      Disp.drawRect(14, 3, 15, 5, 1); //posisi y = 6
      Disp.drawRect(14, 10, 15, 12, 1); //posisi y = 9
    }else{
      Disp.drawRect(14, 3, 15, 5, 0); //posisi y = 5
      Disp.drawRect(14, 10, 15, 12, 0); //posisi y = 8
    }
  }

  // Animasi gerakan teks (y)
  if (run && (Tmr - lsRn_y) > 55) {
    lsRn_y = Tmr;

    if (s == 0 && y < 9) {
      y++;
    } else if (s == 1 && y > 0) {
      y--;
    }
  }

  // Delay sebelum animasi keluar (reverse)
  if (y == 9 && s == 0 && tHold == 0) {
    tHold = millis();
  }
  if (tHold > 0 && (millis() - tHold > 1500)) {//4000
    s = 1;     // mulai keluar
    tHold = 0; // reset timer
  }

  // Setelah animasi selesai
  if (y == 0 && s == 1) {
    s = 0;
    list = (list + 1) % 7;
    if (list == 0) {
      run = false;
      s1 = 1; // trigger keluar vertikal
      Disp.drawRect(14, 4, 15, 5, 0); //posisi y = 5
      Disp.drawRect(14, 11, 15, 12, 0); //posisi y = 8
    }
  }

// Tampilkan jam digital
  fType(3);
  Disp.drawChar(0, y1 - 17, '0' + now.Hour() / 10); //12
  Disp.drawChar(7, y1 - 17, '0' + now.Hour() % 10);

  Disp.drawChar(17, y1 - 17, '0' + now.Minute() / 10);
  Disp.drawChar(24, y1 - 17, '0' + now.Minute() % 10);

  // Tampilkan teks jadwal sholat
  uint8_t shour = (uint8_t)stime;
  uint8_t sminute = (uint8_t)((stime - shour) * 60);

  if(list == 1){sholatSec[1] = toSecond(shour, sminute); }
  else if(list == 3){sholatSec[2] = toSecond(shour, sminute); }
  else if(list == 4){sholatSec[3] = toSecond(shour, sminute); }
  else if(list == 5){sholatSec[4] = toSecond(shour, sminute); }
  else if(list == 6){sholatSec[5] = toSecond(shour, sminute); }

  char buf[6];
  buf[0] = '0' + shour / 10;
  buf[1] = '0' + shour % 10;
  buf[2] = ':';
  buf[3] = '0' + sminute / 10;
  buf[4] = '0' + sminute % 10;
  buf[5] = '\0';

  fType(1);
  dwCtr(31, y - 9, jadwal[list]);
  dwCtr(32, 18 - y, buf);
  DoSwap = true;
  if (y1 == 0 && s1 == 1) {
    s1 = 0;
    show = ANIM_COUNTER; // ganti mode jika perlu
  }
}*/

//================== animasi jadwal sholat ==============//
void animasiJadwalSholat() {
  if(adzan) return;
  
  RtcDateTime now = Rtc.GetDateTime();
  static int8_t y = 0, y1 = 0;
  static uint8_t s = 0, s1 = 0;
  static bool run = false;
  static uint8_t list = 0;

  if(reset_x != 0){ 
      y = 0;
      y1 = 0;
      s = 0;
      s1 = 0;
      reset_x = 0;
  }

  
  static uint32_t lsRn_y1 = 0;
  static uint32_t lsRn_y = 0;
  static uint32_t tHold = 0;

  uint32_t Tmr = millis();

  // Pilih waktu sholat sesuai list
  float stime;
  switch (list) {
    case 0: stime = JWS.floatImsak; break;
    case 1: stime = JWS.floatSubuh; break;
    case 2: stime = JWS.floatTerbit; break;
    case 3: stime = JWS.floatDzuhur; break;
    case 4: stime = JWS.floatAshar; break;
    case 5: stime = JWS.floatMaghrib; break;
    case 6: stime = JWS.floatIsya; break;
    default: stime = 0; break;
  }

  // Transisi vertikal y1 (jam muncul/hilang)
  if ((Tmr - lsRn_y1) > 55) {
    lsRn_y1 = Tmr;

    if (s1 == 0 && y1 < 17) { y1++; } 
    else if (s1 == 1 && y1 > 0) { y1--; }
  }

  // Saat y1 selesai muncul, mulai animasi jadwal
  if (y1 == 17 && s1 == 0) {
    run = true; 
  }

  // Animasi gerakan teks (y)
  if (run && (Tmr - lsRn_y) > 55) {
    lsRn_y = Tmr;

    if (s == 0 && y < 9) {
      y++;
    } else if (s == 1 && y > 0) {
      y--;
    }
  }

  // Delay sebelum animasi keluar (reverse)
  if (y == 9 && s == 0 && tHold == 0) {
    tHold = millis();
  }
  if (tHold > 0 && (millis() - tHold > 800)) {
    s = 1;     // mulai keluar
    tHold = 0; // reset timer
  }

  // Setelah animasi selesai
  if (y == 0 && s == 1) {
    s = 0;
    list = (list + 1) % 7;
    if (list == 0) {
      run = false;
      s1 = 1; // trigger keluar vertikal
    }
  }

  // Tampilkan teks jadwal sholat
  uint8_t shour = (uint8_t)stime;
  uint8_t sminute = (uint8_t)((stime - shour) * 60);

  if(list == 1){sholatSec[1] = toSecond(shour, sminute); }
  else if(list == 3){sholatSec[2] = toSecond(shour, sminute); }
  else if(list == 4){sholatSec[3] = toSecond(shour, sminute); }
  else if(list == 5){sholatSec[4] = toSecond(shour, sminute); }
  else if(list == 6){sholatSec[5] = toSecond(shour, sminute); }

  char buf[6];
  buf[0] = '0' + shour / 10;
  buf[1] = '0' + shour % 10;
  buf[2] = ':';
  buf[3] = '0' + sminute / 10;
  buf[4] = '0' + sminute % 10;
  buf[5] = '\0';

  fType(0);
  dwCtr(0, y - 9, jadwal[list]);
  fType(1);
  dwCtr(0, 18 - y, buf);
  DoSwap = true;
  if (y1 == 0 && s1 == 1) {
    s1 = 0;
    show = ANIM_JAM; // ganti mode jika perlu
  }
}
//================== animasi jam besar ==================//
void anim_JG()
  {
    if(adzan) return;
    
   RtcDateTime now = Rtc.GetDateTime();
   uint8_t daynow = now.DayOfWeek();
    
    static byte    y;
    static bool    s; // 0=in, 1=out              
    static uint32_t   lsRn;
    uint32_t          Tmr = millis();

    if((Tmr-lsRn)>75) 
      { 
        if(s==0 and y<17){lsRn=Tmr; y++;}
        if(s==1 and y>0){lsRn=Tmr; y--; }
      }
    
    fType(3);

    Disp.drawChar(0, 17 - y, '0' + now.Hour() / 10);
    Disp.drawChar(7, 17 - y, '0' + now.Hour() % 10); 
  
    Disp.drawChar(18, 17 - y, '0' + now.Minute() / 10);
    Disp.drawChar(25, 17 - y, '0' + now.Minute() % 10);
    
    if(now.Second() % 2 ){
      Disp.drawCircle(15,21 - y,1,1);//4
      Disp.drawCircle(15,28 - y,1,1);//11
    }else{
      Disp.drawCircle(15,21 - y,1,0);
      Disp.drawCircle(15,28 - y,1,0);
    }

    if((Tmr-lsRn)>5000 and y ==17) {s=1;}
    if (y == 0 and s==1) {
      Disp.clear();
      Serial.print(F("TIME:"));
      Serial.print(now.Hour());
      Serial.print(F(","));
      Serial.print(now.Minute());
      Serial.print(F(","));
      Serial.print(now.Second());
      Serial.print(F(","));
      Serial.println(daynow);
      s=0; 
      show=ANIM_DATE;
    }
    DoSwap = true; 
    }

void runn(const char* msg, uint8_t speed, uint8_t fontt)
{
  if(adzan) return;
  
  
  static uint32_t x = 0;
  static uint32_t fullScroll = 0;
  static uint32_t lastMs = 0;

  if(reset_x){
    x = 0;
    fullScroll = 0;
    lastMs = 0;
    reset_x = 0;
  }
  
  // ====== Pesan baru → reset animasi ======

  fType(fontt);
  // ====== Pesan kosong → langsung lompat ======
  uint16_t w = Disp.textWidth(msg);
  if (w == 0) {
    nextShowState();
    return;
  }

  // ====== Hitung panjang scroll hanya sekali ======
    fullScroll = w + DWidth;

  uint32_t now = millis();
  if (now - lastMs < speed) return;
  lastMs = now;

  // ====== Animasi scroll ======
  if (x < fullScroll) {
    x++;
  } else {
    x = 0;
    fullScroll = 0;
    nextShowState();
    return;
  }

  Disp.drawText(
    DWidth - x,
    (fontt == 5) ? 0 : 8,
    msg
  );
  DoSwap = true;
}

void dwMrq(const char* msg, uint8_t Speed, uint8_t dDT,uint8_t fontt) //running teks ada jam nya
  { 
    static uint16_t   x; 
    static uint16_t fullScroll = 0;
    if(adzan) return;
    if (reset_x !=0) { x=0; reset_x = 0; fullScroll = 0;}      

    uint32_t          Tmr = millis();
    static uint32_t lss=0;
    RtcDateTime now = Rtc.GetDateTime();
    
    fType(fontt);
    uint16_t w = Disp.textWidth(msg);
    if (w == 0) {
       nextShowState();
       return;
    }
    fullScroll = w + DWidth;
      
    if((Tmr-lss)> Speed)
      { lss=Tmr;
        if (x < fullScroll) {++x;}
        else {
          nextShowState();
          x = 0; 
          fullScroll = 0;
          return;}
     if(dDT==1)
        {
        //fType(1);  //Marquee    jam yang tampil di bawah
        Disp.drawText(DWidth - x, 0, msg); //runing teks diatas
        //fType(1);
        if (x<=6)                     { drawGreg_TS(16-x);}
        else if (x>=(fullScroll-6))   { drawGreg_TS(16-(fullScroll-x));}
        else                          { drawGreg_TS(9);}//posisi jamnya yang bawah
   
        }
     else if(dDT==2) //jam yang diatas
        {    
        //fType(1);
        if (x<=6)                     { drawGreg_TS(x-6);}
        else if (x>=(fullScroll-6))   { drawGreg_TS((fullScroll-x)-6);}
        else                          { drawGreg_TS(0);}  //posisi jam nya yang diatas
        //fType(1); //Marquee  running teks dibawah
        Disp.drawText(DWidth - x, 9 , msg);//runinng teks dibawah
        
        }
      else if(dDT==3) //jam yang diatas
      {
        //fType(1);  //Marquee    jam yang tampil di bawah
        Disp.drawText(DWidth - x, 0, msg); //runing teks diatas
      }
        DoSwap = true;
      }       
  }   

void drawGreg_TS(int8_t y)   // Draw Time
  {
    RtcDateTime now = Rtc.GetDateTime();
    char  Buff[8];
    snprintf(Buff,sizeof(Buff),(now.Second() % 2)?"%02d:%02d":"%02d %02d",now.Hour(),now.Minute());
    dwCtr(0,y,Buff);
    DoSwap = true;
  }
  
void nextShowState()
{ 
  
  switch(show){
    case ANIM_DATE:    show = ANIM_TEXT1; break;
    case ANIM_TEXT1:   show = ANIM_TEXT2; break;
    case ANIM_TEXT2:   show = ANIM_TEXT3; break;
    case ANIM_TEXT3:   show = ANIM_TEXT4; break;
    case ANIM_TEXT4:   show = ANIM_TEXT5; break;
    case ANIM_TEXT5:   show = ANIM_SHOLAT; break;
    //case ANIM_SHOLAT:   show = ANIM_SHOLAT; break;
    //case ANIM_COUNTER: show = ANIM_BIGFONT; break;
  }
}

/*======================= animasi memasuki waktu sholat ====================================*/
void drawAzzan()
{
    //static const char *jadwal[] = {"SUBUH", "DZUHUR", "ASHAR", "MAGRIB","ISYA'"};
    const char *sholat = jadwalAzzan[sholatNow]; 
    static uint8_t ct = 0;
    static uint32_t lsRn = 0;
    uint32_t Tmr = millis();
    const uint8_t limit = config.durasiadzan;

    if (Tmr - lsRn > 500 && ct <= limit)
    {
        lsRn = Tmr;
        if (!(ct & 1))  // Lebih cepat dibandingkan ct % 2 == 0
        {
          fType(1); dwCtr(0, 0, "ADZAN");
          fType(0); dwCtr(0, 8, sholat);
          Buzzer(1);
           
        }
        else
        {
          Buzzer(0);
        }
        ct++;
        DoSwap = true;
    }
    
    if ((Tmr - lsRn) > 1500 && (ct > limit))
    {
        show = ANIM_JAM;
        sholatNow = -1;
        adzan = false;
        ct = 0;
        Buzzer(0);
    }
}

void drawIqomah()  // Countdown Iqomah (9 menit)
{  
    static uint32_t lsRn = 0;
    static uint16_t ct = 0;  // Mulai dari 0 untuk menghindari error
    static uint8_t mnt, scd;
    char locBuff[10];  
    uint32_t now = millis();  // Simpan millis() di awal
    
    uint16_t cn_l = (iqomah[sholatNow] * 60);
    
    mnt = (cn_l - ct) / 60;
    scd = (cn_l - ct) % 60;
    snprintf(locBuff,sizeof(locBuff), "-%02d:%02d", mnt, scd);

   // if ((ct & 1) == 0) {  // Gunakan bitwise untuk optimasi modulo 2
    fType(1);
    dwCtr(0, 0, "MENUJU IQOMAH:");
    dwCtr(0, 8, locBuff);
    DoSwap = true;
    
    if (now - lsRn > 1000) 
    {   
        lsRn = now;
        ct++;

        if (ct > (cn_l - 5)) {
            Buzzer(ct & 1);  // Gunakan bitwise untuk menggantikan modulo 2
        }
    }

    if (ct >= cn_l)  // Pakai >= untuk memastikan countdown selesai dengan benar
    {
        ct = 0;
        Buzzer(0);
        show = ANIM_BLINK;
    }    
}

void blinkBlock()
{
    static uint32_t lsRn;
    static uint16_t ct = 0;
    const uint16_t ct_l = displayBlink[sholatNow] * 60;  // Durasi countdown
    uint32_t noww = millis();  // Simpan millis()

    // Ambil waktu dari RTC
    RtcDateTime now = Rtc.GetDateTime();

    // Hitung countdown
    uint8_t mnt = (ct_l - ct) / 60;
    uint8_t scd = (ct_l - ct) % 60;

    // Tampilkan jam besar
    char timeBuff[9];
    snprintf(timeBuff,sizeof(timeBuff), "%02d:%02d:%02d", now.Hour(), now.Minute(), now.Second());
    
    fType(0);
    dwCtr(0, 8, timeBuff);
    DoSwap = true;
     
    // Update countdown setiap detik
    if (noww - lsRn > 1000)
    {
        lsRn = noww;
        ct++;
    }

    // Reset jika countdown selesai & kembali ke animasi utama
    if (ct > ct_l)
    {
        sholatNow = -1;
        adzan = false;
        ct = 0;
        show = ANIM_JAM;
    }
}
//===================================== end =================================//



void dwCtr(int8_t x, int8_t y, const char* Msg){
   uint16_t   tw = Disp.textWidth(Msg);
   int16_t   c = int16_t((DWidth-x-tw)/2);
   Disp.drawText(x+c,y,Msg);
}
  
void fType(int x)
  {
    if(x==0) Disp.setFont(Font0);
    else if(x==1) Disp.setFont(Font1); 
    else if(x==2) Disp.setFont(Font2);
    else if(x==3) Disp.setFont(Font3);
    else if(x==4) Disp.setFont(Font4);
    else if(x==5) Disp.setFont(Font5);
    else if(x==6) Disp.setFont(Font6); 
    else if(x==7) Disp.setFont(Font7); 
//    else if(x==8) Disp.setFont(Font8);
  }











  
