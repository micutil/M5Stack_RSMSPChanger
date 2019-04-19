/*************************************************
 * RSMSPChanger
 * 
 * LISENCE
 *  CC 4.0 BY-NC-ND Micono Utilities (Michio Ono)
 *  
 *  About Creative Commons
 *  https://creativecommons.org/licenses/by-nc-nd/4.0/
 * 
 *************************************************/

/*************************************************************

    M5Stackで日本語表示
    https://qiita.com/taront/items/7900c88b9e9782c33b08

    茶虎たま吉さんのArduino-KanjiFont-Library-SD
    https://github.com/Tamakichi/Arduino-KanjiFont-Library-SD

*/

#include <M5Stack.h>
#include "M5StackUpdater.h" //for SD updater
#include "DispJPFont.h"
#include "FS.h"
#include "SD.h"

#define SD_PN 4

int cmsp[4];
int cmsmax[4];
int pnhs=1;

int working=0;
int selecting=0;

int selmode=0;
int updwnstep=1;

int smpX = 40;
int smpS = 96;
int smpY = 220;
int fntsz = 12;

void DrawButtonName() {
  M5.Lcd.fillRect(0, smpY, 320, 240-smpY, 0);
  
  SDfonts.open(); // フォントのオープン
  switch (selmode) {
    case 0:
    case 1:
    case 2:
    case 3:
      fontDump(smpX, smpY, " ↓  ", fntsz, TFT_WHITE, false);
      fontDump(smpX + smpS, smpY, " OK ", fntsz, TFT_GREEN, false);
      fontDump(smpX + smpS * 2, smpY, " ↑  ", fntsz, TFT_WHITE, false);
      break;
    case 4:
      fontDump(smpX, smpY, "やり直す", fntsz, TFT_RED, false);
      fontDump(smpX + smpS, smpY, "書換え", fntsz, TFT_YELLOW, false);
      fontDump(smpX + smpS * 2, smpY, "接続調べ", fntsz, TFT_WHITE, false);
      break;
  }
  SDfonts.close(); // フォントのクローズ  
}


void DrawMessage() {
  M5.Lcd.fillRect(0, 60, 320, smpY-60, 0);
  SDfonts.open(); // フォントのオープン

  String s="";
  switch(selmode) {
    case 0://コンプライアンスマージン
      fontDump(20, 70, "コンプライアンスマージン", 14, TFT_YELLOW);
      s="単位 (0.1度) = "+String(cmsp[selmode]);
      fontDump(20, 90, (char*)s.c_str(), 14, TFT_WHITE);
      break;

    case 1://コンプライアンススロープ
      fontDump(20, 70, "コンプライアンススロープ", 14, TFT_YELLOW);
      s="単位 (1度) = "+String(cmsp[selmode]);
      fontDump(20, 90, (char*)s.c_str(), 14, TFT_WHITE);
      break;

    case 2://パンチ
      fontDump(20, 70, "パンチ", 14, TFT_YELLOW);
      s="単位 (約0.4%) = "+String(cmsp[selmode]);
      fontDump(20, 90, (char*)s.c_str(), 14, TFT_WHITE);
      break;
      
    case 3://サーボID
      fontDump(20, 70, "対象サーボID", 14, TFT_YELLOW);
      s="ID (255:全サーボ) = "+String(cmsp[selmode]);
      fontDump(20, 90, (char*)s.c_str(), 14, TFT_WHITE);
      break;
      
    case 4:
      s="マージン = "+String(cmsp[0]);
      fontDump(20, 70, (char*)s.c_str(), 14, TFT_WHITE);
      s="スロープ = "+String(cmsp[1]);
      fontDump(20, 90, (char*)s.c_str(), 14, TFT_WHITE);
      s="パンチ = "+String(cmsp[2]);
      fontDump(20,110, (char*)s.c_str(), 14, TFT_WHITE);
      s="サーボ = "+String(cmsp[3]);
      fontDump(20,130, (char*)s.c_str(), 14, TFT_WHITE);
      break;
      
  }
  SDfonts.close(); // フォントのクローズ
  
  DrawButtonName();
}


void trqueONOFF(int n) {
  byte trq[]={0xFA, 0xAF, 0x00, 0x00, 0x24, 0x01, 0x01, 0x01, 0x25};
  trq[2]=cmsp[3];
  M5.Lcd.fillRect(0, 170, 320, 20, 0);
  if(n>0) {
    //トルクON
    fontDump(20,170, "トルクオン", 14, TFT_WHITE);
    //CHR$(T,U,V,0,X,1,1,1,#25^V)
    trq[5]=1;
    trq[8]=0x25^trq[2];
  } else {
    fontDump(20,170, "トルクオフ", 14, TFT_WHITE);
    //CHR$(T,U,V,0,X,1,1,0,#24^V)
    trq[5]=0;
    trq[8]=0x24^trq[2];
  }
  Serial2.write(trq,9);
  delay(500);
}

void rotateServo(int n) {
  byte rot[]={0xFA, 0xAF, 0xFF, 0x00, 0x1E, 0x02, 0x01, 0x00, 0x00, 0xE2};
  rot[2]=cmsp[3];
  M5.Lcd.fillRect(0, 170, 320, 20, 0);
  String s="角度="+String(n/10);
  fontDump(20,170,(char*)s.c_str(), 14, TFT_WHITE);
  //CHR$(T,U,V,0,Y,2,1,A,B,C)
  //A=Q&#FF:B=(Q>>8)&#FF:C=((#1D^V)^A)^B
  rot[7]=n&0xFF;
  rot[8]=(n>>8)&0xFF;
  rot[9]=((0x1D^rot[2])^rot[7])^rot[8];
  Serial2.write(rot,10);
  delay(1000);
}

void compMagin(int cw, int n) {
  byte mgn[]={0xFA, 0xAF, 0xFF, 0x00, 0x18, 0x01, 0x01, 0x01, 0x18};
  mgn[2]=cmsp[3];  
  M5.Lcd.fillRect(0, 170, 320, 20, 0);
  String s="マージン="+String(n);
  if(cw==0) {
    s="CW"+s;
  } else {
    s="CCW"+s;
  }
  fontDump(20,170,(char*)s.c_str(), 14, TFT_WHITE);
  //CHR$(T,U,V,0,A,1,1,X,(A^X)^V)
  mgn[4]=0x18+cw;
  mgn[7]=n;
  mgn[8]=(mgn[4]^n)^mgn[2];
  Serial2.write(mgn,9);
  delay(1000);
}

void compSlop(int cw, int n) {
  byte slp[]={0xFA, 0xAF, 0xFF, 0x00, 0x1A, 0x01, 0x01, 0x01, 0x1A};
  slp[2]=cmsp[3];
  M5.Lcd.fillRect(0, 170, 320, 20, 0);
  String s="スロープ="+String(n);
  if(cw==0) {
    s="CW"+s;
  } else {
    s="CCW"+s;
  }
  fontDump(20,170,(char*)s.c_str(), 14, TFT_WHITE);
  //CHR$(T,U,V,0,A,1,1,X,(A^X)^V)
  slp[4]=0x1A+cw;
  slp[7]=n;
  slp[8]=(slp[4]^n)^slp[2];
  Serial2.write(slp,9);
  delay(1000);
}

void panch(int n) {
  byte pnc[]={0xFA, 0xAF, 0xFF, 0x00, 0x1C, 0x02, 0x01, 0x00, 0x00, 0x1F};
  pnc[2]=cmsp[3];
  M5.Lcd.fillRect(0, 170, 320, 20, 0);
  String s="パンチ="+String(n);
  fontDump(20,170,(char*)s.c_str(), 14, TFT_WHITE);
  //CHR$(T,U,V,0,#1C,2,1,A,B,C)
  //A=X&#FF:B=(X>>8)&#FF:C=((#1F^V)^A)^B
  pnc[7]=n&0xFF;
  pnc[8]=(n>>8)&0xFF;
  pnc[9]=((0x1F^pnc[2])^pnc[7])^pnc[8];
  Serial2.write(pnc,10);
  delay(1000);
}

void writeRom() {
  byte wrm[]={0xFA, 0xAF, 0x00, 0x40, 0xFF, 0x00, 0x00, 0xBF};
  wrm[2]=cmsp[3];
  M5.Lcd.fillRect(0, 170, 320, 20, 0);
  fontDump(20,170,"ROMに書き込み", 14, TFT_WHITE);
  //CHR$(T,U,V,#40,#FF,0,0,#BF^V)
  wrm[7]=0xBF^wrm[2];
  Serial2.write(wrm,8);
  delay(3000);
}

void restertRom() {
  byte rrm[]={0xFA, 0xAF, 0x00, 0x20, 0xFF, 0x00, 0x00, 0xDF};
  rrm[2]=cmsp[3];
  fontDump(20,170,"サーボ再起動", 14, TFT_WHITE);
  //CHR$(T,U,V,#20,#FF,0,0,#DF^V)
   rrm[7]=0xDF^rrm[2];
  Serial2.write(rrm,8);
  delay(2000);
}

void setup() {
  // put your setup code here, to run once:
  M5.begin();

  // put your setup code here, to run once:
  M5.begin();
  //Wire.begin();
  if (digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }
  
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  //Serial2.begin(115200, SERIAL_8N1, 17, 16);
  delay(500);

  M5.Lcd.setTextSize(2);

  M5.Lcd.setBrightness(100);
  M5.Lcd.fillScreen(TFT_BLACK);

  SDfonts.init(SD_PN);
  Serial.println(F("sdfonts liblary"));

  //M5Stack IJUploader
  fontDump(20, 5, "MSPChanger", 24, TFT_CYAN);
  fontDump(20, 30, "マージン、スロープ、パンチ設定変更", 16);
  fontDump(20, 47, "by Micono v1.0", 12, TFT_GREEN);

  cmsp[0]=cmsp[1]=1;
  cmsp[2]=8;
  cmsp[3]=255;
  
  cmsmax[0]=255;
  cmsmax[1]=255;
  cmsmax[2]=255;
  cmsmax[3]=255;
  
  DrawMessage();
}

void loop() {
  // put your main code here, to run repeatedly:
  M5.update();

  if(working>0) {
  } else {
    if(M5.BtnA.wasPressed()) {
      //Down
      doBtnAC(-1);
    }
    if(M5.BtnB.wasPressed()) {
      //Select
      doBtnB();
    }
    if(M5.BtnC.wasPressed()) {
      //Up
      doBtnAC(1);
    }
  }
}

void doBtnAC(int n) {
  switch(selmode) {
    case 0://マージン
    case 1://スロープ
    case 2://パンチ
    case 3://サーボID
      cmsp[selmode]=cmsp[selmode]+n*pnhs;
      if(cmsp[selmode]<0) {
        cmsp[selmode]=cmsmax[selmode];
      } else if(selmode==3&&cmsp[selmode]>cmsmax[selmode]) {
        cmsp[selmode]=1;
      } else if(selmode==3&&cmsp[selmode]<1) {
        cmsp[selmode]=255;
      } else if(selmode==3&&n==1&&cmsp[selmode]<255&&cmsp[selmode]>128) {
        cmsp[selmode]=255;
      } else if(selmode==3&&n==-1&&cmsp[selmode]<255&&cmsp[selmode]>128) {
        cmsp[selmode]=128;
      } else if(cmsp[selmode]>cmsmax[selmode]) {
        cmsp[selmode]=0;
      }
      DrawMessage();
      break;
    case 4:
      if(n==1) {
        //回転
        working=1;
        trqueONOFF(1);//ON
        rotateServo(100);
        rotateServo(-100);
        rotateServo(100);
        rotateServo(0);
        trqueONOFF(0);//OFF
        working=0;
      } else {
        //最初にもどる
        selmode=0;
      }
      DrawMessage();
      break;
  }
}

void doBtnB() {
  switch(selmode) {
    case 0:
    case 1:
    case 2:
    case 3:
      selmode++;
      DrawMessage();
      break;
    case 4:
      working=1;
      trqueONOFF(0);//OFF
      compMagin(0,cmsp[0]);
      compMagin(1,cmsp[0]);
      compSlop(0,cmsp[1]);
      compSlop(1,cmsp[1]);
      panch(cmsp[2]);
      writeRom();
      restertRom();
      working=0;
      selmode=0;
      DrawMessage();
      break;
  }
}
