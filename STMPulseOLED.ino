


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


//Encoder
#define encA PB0
#define encB PB1

#define enc2A PB10 //11
#define enc2B PB11 //10
#define BUT1 PA0
#define BUT2 PB15
#define BUT3 PB7
#define BUTLED3 PA12
#define START PA2
#define RESET PA1

int DACS[2] = {PB6, PB7};
char Modes[] = {'P', 'L', 'N', 'O', 'M'};
byte Mode = 0;

int encAVal, encALast, encBVal;
int encAVal2, encALast2, encBVal2;

HardwareTimer SequenceTimer(1);
HardwareTimer SequenceGateTimer(2);
HardwareTimer BeatTimer(3);
HardwareTimer BeatGateTimer(4);


int Tempo = 120;
long BaseTime = 1000000L * 60 / Tempo;
long BeatGateTime = 1000000L * 60 / Tempo;
short NoteLength = 4;
long SeqGateTime = BeatGateTime / NoteLength;
long ms15 = 0015000L;
short Timers[] = {3, 3, 3, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1};
short BeatPos[] = {0, 3, 5, 6, 7, 9, 11, 12, 13};
short Notes[] = {0, 2, 5, 0, 8, 0, 0, 10, 6, 2, 0, 10, 8};
short Octave[] = {1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0};
int count = 0;
int Beat = 1;
int MaxBeat = 8;
int countlength = 13;
int Gate = PB5;
int ErasePos = 0;
short encPos = Tempo;
short NewEncPos = Tempo;
short encPos2 = NoteLength;
short NewEncPos2 = NoteLength;




float Range = 819.2; // (2^12/5)
int DacTest = 0;

volatile int hKey ;
volatile int hOctave;
volatile int hNote;
volatile int houtValue;




void SetupEncoders() {
  pinMode(encA, INPUT);
  pinMode(encB, INPUT);
  pinMode(enc2A, INPUT);
  pinMode(enc2B, INPUT);

  //digitalWrite(encA, HIGH);
  //digitalWrite(encB, HIGH);
  //digitalWrite(enc2A, HIGH);
  //digitalWrite(enc2B, HIGH);
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

void SetupDacs() {
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  pinMode(DACS[0], OUTPUT);
  pinMode(DACS[1], OUTPUT);
  digitalWrite(DACS[0], HIGH);
  digitalWrite(DACS[1], HIGH);
}

void SetupSwitches() {
  pinMode(BUT1, INPUT);
  pinMode(BUT2, INPUT);
  pinMode(BUT3, INPUT);
  pinMode(BUTLED3, OUTPUT);
  pinMode(START, INPUT);
  pinMode(RESET, INPUT);
  attachInterrupt(BUT1, iMode, FALLING);
  attachInterrupt(BUT2, iMode, FALLING);
  attachInterrupt(BUT3, interrupt, FALLING);

  attachInterrupt(START, interrupt, RISING);

  attachInterrupt(RESET, IntReset, RISING);

  pinMode(Gate, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}


void setup() {

  SetupEncoders();
  SetupOLED();
  SetupDacs();
  SetupSwitches();

  DisplayBackground();
  DisplayTempo();
  DisplayMode();
  DisplayNoteLength();

  SequenceGateTimer.pause();
  BeatGateTimer.pause();
  SequenceTimer.setCount(0);
  SequenceTimer.pause();
  BeatTimer.setCount(0);
  BeatTimer.pause();
}


void loop() {
  handleEnc1();
  handleEnc2();
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

  WriteNote(Notes[count], Octave[count], 0);

  // Display count
  DisplayCount(count, 0, 0);

  //Display Cursor
  Cursor(9 * count, 14);
  DrawMode(NORMAL);
  _WriteChar('_' - 32);
  ErasePos = count - 1;
  if (ErasePos < 0)
    ErasePos = countlength - 1;
  Cursor(9 * ErasePos, 14);
  DrawMode(CLEAR);
  _WriteChar('_' - 32);
  count++;
  if (count >= countlength)
    count = 0;
  Refresh();
}


void BeatGateOn() {
  digitalWrite(Gate, true);
  digitalWrite(BUTLED3, true);
  BeatTimer.setPeriod(BeatGateTime);
  BeatTimer.refresh();
  BeatGateTimer.refresh();
  BeatGateTimer.resume();
  BeatTimer.resume();


  DisplayCount(Beat, 14, 0);

  Cursor(9 * BeatPos[Beat - 1], 16);
  DrawMode(NORMAL);
  _WriteChar('_' - 32);

  ErasePos = Beat - 1;
  if (ErasePos <= 0)
    ErasePos = MaxBeat;

  Cursor(9 * BeatPos[ErasePos - 1], 16);
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
  digitalWrite(BUTLED3, false);

}
void SequenceBeatGateOff() {
  SequenceGateTimer.pause();
  digitalWrite(LED_BUILTIN, false);

}





void Start() {
  SequenceTimer.attachInterrupt(1, SequenceGateOn);
  SequenceGateTimer.attachInterrupt(2, SequenceBeatGateOff);
  BeatTimer.attachInterrupt(3, BeatGateOn);
  BeatGateTimer.attachInterrupt(4, BeatGateOff);
  Stop();
  SequenceGateTimer.setPeriod(SeqGateTime);
  BeatGateTimer.setPeriod(ms15);
  SequenceGateOn();
  BeatGateOn();
}

void Stop() {
  SequenceGateTimer.pause();
  BeatGateTimer.pause();
  SequenceTimer.setCount(0);
  SequenceTimer.pause();
  BeatTimer.setCount(0);
  BeatTimer.pause();
}

boolean Running = false;
void interrupt() {
  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  Running = !Running; //invert state
  if (Running) {
    Start();
  } else {
    Stop();
  }


  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

}

void iMode() {
  Mode++;
  if (Mode > 4) {
    Mode = 0;

  }
  DisplayMode();
}

void IntReset() {
  count = 0;
}



void setTempo(int newTempo) {
  Tempo = newTempo;
  BaseTime = 1000000L * 60 / Tempo;
  BeatGateTime = 1000000L * 60 / Tempo;

}



int encodeVal2(int val) {
  encAVal2 = digitalRead(enc2A);
  encBVal2 = digitalRead(enc2B);
  if (encAVal2 != encALast2) {
    if (encAVal2 == encBVal2) {
      val--;
    } else {
      val++;
    }
    encALast2 = encAVal2;

  }
  return val;
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
    switch (Mode) {
      case 0: encALast = encAVal;
        break;
      default: break;
    }
  }
  return val;
}
void handleEnc1() {

  NewEncPos = encodeVal(encPos);
  if (NewEncPos != encPos) {
    switch (Mode) {
      case 0: encPos = NewEncPos;
        if (encPos < 1) {
          encPos = 1;
        }
        if (encPos > 180 ) {
          encPos = 180;
        }
        setTempo(encPos);
        DisplayTempo() ;
        break;
      default: break;
    }

  }
}


void handleEnc2() {

  NewEncPos2 = encodeVal2(encPos2);
  if (NewEncPos2 != encPos2) {
    switch (Mode) {
      case 0:
        encPos2 = NewEncPos2;
        if (encPos2 < 1) {
          encPos2 = 1;
        }
        if (encPos2 > 8 ) {
          encPos2 = 8;
        }
        NoteLength = encPos2;
        SeqGateTime = BeatGateTime / NoteLength;
        SequenceGateTimer.setPeriod(SeqGateTime);
        DisplayNoteLength() ;
        break;
      default: break;
    }
  }
}


int CX = 0;
int CY = 0;
int Width = 8;
int Height = 8;


void DisplayChar(char C, byte x, byte y) {
  CX = Width * x;
  CY = Height * y;
  Cursor(CX, CY);
  Erase(CX, CY, CX + Width , CY + Height);
  _WriteChar(C);
}

//Display two digit integer
void DisplayCount(byte Count, byte x, byte y) {
  Count++;

  byte low = 16 + Count % 10;
  byte high = 16 + Count / 10;
  if (high != 0) {
    DisplayChar(high, x, y);
  }
  DisplayChar(low, x + 1, y);
  Refresh();

}
void DisplayMode() {
  DisplayChar(Modes[Mode]-32, 4, 0);
  Refresh();

}

void DisplayTempo() {
  int CX = 7 * 8;
  int CY = 0;
  Cursor(CX, CY);
  Erase(CX, CY, CX + Width * 3, CY + Height);
  Print(Tempo);
  Refresh();
}

void DisplayNoteLength() {
  CX = 12 * 8;
  Cursor(CX , CY);
  Erase(CX , CY, CX + Width , CY + Height);
  Print(9 - NoteLength);
  Refresh();
}

void DisplayBackground() {
  CX = 54;
  CY = 0;
  Line(CX - 1, CY, CX - 1, CY + Height + 1);
  Line(CX - 1, CY + Height + 1, 78, CY + Height + 1);
  Line(1 + CX + Width * 3, 0, 1 + CX + Width * 3, CY + Height + 1);
  int i = 0;

  for (i = 0; i < countlength; i++) {
    Cursor(i * 9, 12);
    _WriteChar('~' - 32 + Timers[i]);
  }

  for (i = 0; i < countlength; i++) {
    Cursor(i * 9, 24);
    _WriteChar('~' - 32 + 7 + Notes[i]);
  }

  for (i = 0; i < countlength; i++) {
    Cursor(i * 9, 33);
    _WriteChar('0' - 32 + Octave[i]);
  }

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

void WriteNote(int Note, int Octave, int Channel) {
  hOctave = Octave;
  hNote = (byte)(Note % 12);
  houtValue = (int)(Range * (hOctave + (float)hNote / 12));


  mcpWrite(houtValue, Channel, 0);
}

