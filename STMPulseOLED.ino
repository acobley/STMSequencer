


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

//SW
#define sw 5 //9

//Encoder
#define encA PB0 //11
#define encB PB1 //10
int DACS[2] = {PB6, PB7};

int encAVal, encALast, encBVal;


HardwareTimer SequenceTimer(1);
HardwareTimer SequenceGateTimer(2);
HardwareTimer BeatTimer(3);
HardwareTimer BeatGateTimer(4);


int Tempo = 121;
long BaseTime = 1000000L * 60 / Tempo;
long BeatGateTime = 1000000L * 60 / Tempo;
short Timers[] = {3, 3, 3, 2, 2, 1, 1,2,2,2,2,1,1};
short BeatPos[] = {0, 3, 5, 6,7,9,11,12,13};
short Notes[]={0,2,5,0,8,0,0,10,6,2,0,10,8};
int count = 0;
int Beat = 1;
int MaxBeat=8;
int countlength = 13;
int Gate = PB5;
int ErasePos = 0;
short encPos = Tempo;
short NewEncPos = Tempo;


float Range = 819.2; // (2^12/5)
int DacTest=0;

volatile int hKey ;
volatile int hOctave;
volatile int hNote;
volatile int houtValue;
void WriteNote(int Note, int Channel){
  hOctave = (byte)(Note / 12);
      hNote = (byte)(Note % 12);
      houtValue = (int)(Range * (hOctave + (float)hNote / 12));
  
      
      mcpWrite(houtValue,Channel,0);
}

void SequenceGateOn() {
  digitalWrite(LED_BUILTIN, true);
  long Time = (long)(BaseTime / Timers[count]);
  SequenceTimer.setPeriod(Time);
  SequenceTimer.refresh();
  SequenceTimer.resume();
  SequenceGateTimer.refresh();
  SequenceGateTimer.resume();

//  Deal with notes

  WriteNote(Notes[count],0);
  
  Cursor(5, 0);
  Erase(5, 0, 5 + 8 * 5, 0 + 8);
  Print(count + 1);
  
 
  
  Cursor(9 * count, 22);
  DrawMode(NORMAL);
  _WriteChar('_' - 32);
  ErasePos = count - 1;
  if (ErasePos < 0)
    ErasePos = countlength - 1;
  Cursor(9 * ErasePos, 22);
  DrawMode(CLEAR);
  _WriteChar('_' - 32);
  count++;
  if (count >= countlength)
    count = 0;




  Refresh();
}


void BeatGateOn() {
  digitalWrite(Gate, true);

  BeatTimer.setPeriod(BeatGateTime);
  BeatTimer.refresh();
  BeatGateTimer.refresh();
  BeatGateTimer.resume();
  BeatTimer.resume();

  Cursor(15 * 8, 0);
  Erase(15 * 8, 0, 15 * 8 + 8 * 3, 0 + 8);
  Print(Beat);

  Cursor(9 * BeatPos[Beat - 1], 24);
  DrawMode(NORMAL);
  _WriteChar('_' - 32);

  ErasePos = Beat - 1;
  if (ErasePos <= 0)
    ErasePos = MaxBeat;

  Cursor(9 * BeatPos[ErasePos - 1], 24);
  DrawMode(CLEAR);
  _WriteChar('_' - 32);

  Refresh();
  Beat++;
  if (Beat > MaxBeat)
    Beat = 1;


}

void BeatGateOff() {
  BeatGateTimer.pause();
  digitalWrite(Gate, false);

}
void SequenceBeatGateOff() {
  SequenceGateTimer.pause();
  digitalWrite(LED_BUILTIN, false);

}

void interrupt() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void SetupEncoders() {
  pinMode(encA, INPUT);
  pinMode(encB, INPUT);
  digitalWrite(encA, HIGH);
  digitalWrite(encB, HIGH);
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

  DrawMode(CLEAR);
  for (int x = 0; x < 128; x++) {
    for (int y = 0; y < 64; y++) {
      Plot(x, y);
    }
    Refresh();
  }

  Erase(0, 0, 128, 64);
  Refresh();

}

void SetupDacs(){
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  pinMode(DACS[0], OUTPUT);
  pinMode(DACS[1], OUTPUT);
  digitalWrite(DACS[0], HIGH);
  digitalWrite(DACS[1], HIGH);
}

void setup() {
  pinMode(Gate, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  SetupEncoders();
  SetupOLED();
  SetupDacs();

  DisplayBackground();
  DisplayTempo();

  SequenceGateTimer.pause();
  BeatGateTimer.pause();
  SequenceTimer.setCount(0);
  SequenceTimer.pause();
  BeatTimer.setCount(0);
  BeatTimer.pause();


  SequenceTimer.attachInterrupt(1, SequenceGateOn);
  SequenceGateTimer.attachInterrupt(2, SequenceBeatGateOff);
  BeatTimer.attachInterrupt(3, BeatGateOn);
  BeatGateTimer.attachInterrupt(4, BeatGateOff);

  SequenceGateTimer.setPeriod(0015000);
  BeatGateTimer.setPeriod(0015000);
  SequenceGateOn();
  BeatGateOn();
  
}

void setTempo(int newTempo) {
  Tempo = newTempo;
  BaseTime = 1000000L * 60 / Tempo;
  BeatGateTime = 1000000L * 60 / Tempo;

}

void handleEnc1() {

  NewEncPos = encodeVal(encPos);
  if (NewEncPos != encPos) {
    encPos = NewEncPos;
    if (encPos < 1) {
      encPos = 1;
    }
    if (encPos > 180 ) {
      encPos = 180;
    }
    setTempo(encPos);
    DisplayTempo() ;
  }
}

  void loop() {
    handleEnc1();
   
    
  }

  int CX = 54;
  int CY = 0;
  int Width = 8;
  int Height = 8;
  void DisplayBackground() {
    Line(CX - 1, CY, CX - 1, CY + Height + 1);
    Line(CX - 1, CY + Height + 1, 78, CY + Height + 1);
    Line(1 + CX + Width * 3, 0, 1 + CX + Width * 3, CY + Height + 1);
    int i = 0;
    Cursor(0, 20);
    for (i = 0; i < countlength; i++) {
      _WriteChar('~' - 32 + Timers[i]);
    }
  }


  void DisplayTempo() {

    Cursor(CX, CY);
    Erase(CX, CY, CX + Width * 3, CY + Height);
    Print(Tempo);
    Refresh();
  }


  int encodeVal(int val) {
    encAVal = digitalRead(encA);
    encBVal = digitalRead(encB);
    if (encAVal != encALast) {
      if (encAVal == encBVal) {
        val--;
      } else {
        val++;
      }
      encALast = encAVal;
    }
    return val;
  }

void mcpWrite(int value, int DAC, int Channel) {
  //CS
  noInterrupts();
  digitalWrite(DACS[DAC], LOW);
  //DAC1 write
  //set top 4 bits of value integer to data variable
  byte data = value >> 8;
  data = data & B00001111;
  if (Channel == 0)
    data = data | B00110000; //DACA Bit 15 Low
  else
    data = data | B10110000; //DACB Bit 15 High
  SPI.transfer(data);
  data = value;
  SPI.transfer(data);
  // Set digital pin DACCS HIGH
  digitalWrite(DACS[DAC], HIGH);
  interrupts();


}

