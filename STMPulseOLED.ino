


/*
   Contains code from HobbyComponents.com

   You may copy, alter and reuse this code in any way you like, but please leave
  reference to HobbyComponents.com in your comments if you redistribute this code.
  This software may not be used directly for the purpose of promoting products that
  directly compete with Hobby Components Ltd's own range of products.

  THIS SOFTWARE IS PROVIDED "AS IS". HOBBY COMPONENTS MAKES NO WARRANTIES,
  WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ACCURACY OR
  LACK OF NEGLIGENCE. HOBBY COMPONENTS SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE
  FOR ANY DAMAGES INCLUDING, BUT NOT LIMITED TO, SPECIAL, INCIDENTAL OR
  CONSEQUENTIAL DAMAGES FOR ANY REASON WHATSOEVER.



*/
#include "HardwareTimer.h"
#include "SPI.h"
#include "fonts.h"


int Tempo = 120;
float fTime = 1.0 / (Tempo / 60);
long BaseTime = 1000000 * 60/Tempo;
long GateTime = 1000000 * 60 /Tempo;
short Timers[] = {3, 3, 3, 2, 2, 1, 4,4, 2};
int count = 0;
int Beat = 0;
int countlength = 9;
int Gate = PB5;

void LedOn() {
  digitalWrite(LED_BUILTIN, true);
  long Time = (long)(BaseTime / Timers[count]);
  Timer1.setPeriod(Time);
  Timer1.setCount(0);
  Timer2.setCount(0);
  Timer2.resume();
  Timer1.resume();
  Cursor(10, 0);
  Erase(10, 0, 10 + 8 * 3, 0+ 8);
  Print(count);
  Cursor(9*count,26+10);
  _DrawMode=CLEAR;
  _WriteChar('_'-32);
  
  count++;
  if (count >= countlength)
    count = 0;
  Cursor(9*count,26+10);
  _DrawMode=NORMAL;
  _WriteChar('_'-32);
  Refresh();
}


void GateOn() {
  digitalWrite(Gate, true);

  Timer3.setPeriod(GateTime);
  Timer3.setCount(0);
  Timer4.setCount(0);
  Timer4.resume();
  Timer3.resume();
  Cursor(15 * 8, 0);
  Erase(15* 8, 0, 15 * 8 + 8 * 3, 0 + 8);
  Print(Beat);
  Refresh();
  Beat++;
  if (Beat > 4)
    Beat = 1;
}

void GateOff() {
  Timer4.pause();
  digitalWrite(Gate, false);

}
void LedOff() {
  Timer2.pause();
  digitalWrite(LED_BUILTIN, false);

}

void interrupt() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}




void SetupOLED() {
  _Res_Max_X = SH1106_RES_X;
  _GRAM_Col_Start = SH1106_GRAM_COL_START;
  _GRAM_Col_End = SH1106_GRAM_COL_END;
  _GRAM_Page_Start = SH1106_GRAM_PAGE_START;
  _GRAM_Page_End = SH1106_GRAM_PAGE_END;
  _RAM_Pages = SH1106_GRAM_PAGE_END - SH1106_GRAM_PAGE_START + 1;

  SPI.begin();

  SPI.setBitOrder(MSBFIRST);
  pinMode(CS_DI, OUTPUT);
  pinMode(DC_DI, OUTPUT);
  pinMode(RST_DI, OUTPUT);

  Reset();
  Cursor(0, 0);
  DrawMode(NORMAL);
  _FontType = Terminal_8pt;
  _FontHight = Terminal_8ptFontInfo.CharacterHeight;
  _FontDescriptor = Terminal_8ptFontInfo.Descriptors;
  for (int x = 0; x < 128; x++) {
    for (int y = 0; y < 64; y++) {
      Plot(x, y);
    }
    Refresh();
  }

  
  delay(1000);
  Erase(0, 0, 128, 64);
  Refresh();

}

void setup() {
  pinMode(Gate, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Timer2.setPeriod(0015000);
  Timer3.setPeriod(GateTime);
  Timer4.setPeriod(0015000);
  Timer1.attachInterrupt(0, LedOn);
  Timer2.attachInterrupt(0, LedOff);
  Timer3.attachInterrupt(0, GateOn);
  Timer4.attachInterrupt(0, GateOff);
  Timer2.pause();
  Timer4.pause();
  Timer1.setCount(0);
  Timer1.pause();
  Timer3.setCount(0);
  Timer3.pause();
  SetupOLED();
  LedOn();
  GateOn();
  DisplayBackground();
  DisplayTempo();
  
}

void loop() {

}

int CX=54;
int CY=0;
int Width=8;
int Height=8;
void DisplayBackground(){
   Line(CX-1,CY,CX-1,CY+Height+1);
   Line(CX-1,CY+Height+1,78,CY+Height+1);
   Line(1+CX + Width * 3,0,1+CX + Width * 3,CY+Height+1);
   int i=0;
   Cursor(0,26+9);
   for (i=0;i<countlength;i++){
       _WriteChar('~'-32+Timers[i]);
   }
}


void DisplayTempo(){
  
  Cursor(CX, CY);
  Erase(CX, CY, CX + Width * 3, CY + Height);
  Print(Tempo);
  Refresh();
}



