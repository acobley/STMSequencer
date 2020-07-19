


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

/*
    To setup BluePill:

    https://circuitdigest.com/microcontroller-projects/getting-started-with-stm32-development-board-stm32f103c8-using-arduino-ide

*/


#include "HardwareTimer.h"
#include "SPI.h"
#include "fonts.h"
#include <EEPROM.h>




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

int DAC = PB6; //????
int Gate1 = PB5;
int Gate2 = PC13;
byte CV1 = 1;
byte CV2 = 0;
char Modes[] = {'P', 'L', 'N', 'O', 'M'};
char Modes2[] = {'P', 'S'};
byte Mode = 0;
byte Mode2 = 0;

int encAVal, encALast, encBVal;
int encAVal2, encALast2, encBVal2;

HardwareTimer SequenceTimer(1);
HardwareTimer SequenceGateTimer(2);
HardwareTimer BeatTimer(3);
HardwareTimer BeatGateTimer(4);



int Tempo = 120;
long BaseTime = 1000000L * 60 / Tempo;
long BeatGateTime = 1000000L * 60 / Tempo;
short GateLength = 4;
long SeqGateTime = BeatGateTime / GateLength;
long ms15 = 0015000L;


int count = 0;
int ErasePos = -1;
int bErasePos;
int Beat = 1;
int MaxBeat = 10;

float NumberOfBeats = 1.0;
short CurrentPattern = 0;
short NextPattern = -1;

const short MaxPatternLength = 16;
const short NumPatterns = 6;
short Timers[MaxPatternLength] ;
short calcBeatPos[MaxPatternLength];
short BeatPos[MaxPatternLength] ;
short Notes[MaxPatternLength] ;
short Octave[MaxPatternLength] ;
short Volume[MaxPatternLength] ;
int countlength = sizeof(Notes) / sizeof(short);

short PatternLengths[NumPatterns] = {13, 13, 4, 5,4, 5};
short NotePatterns [ NumPatterns][MaxPatternLength] = {
  {10, 8, 1, 3, 8, 5, 3, 8, 5, 1, 3, 10, 8},
  {10, 8, 1, 3, 8, 5, 3, 8, 5, 1, 3, 10, 8},
  {3, 3, 10, 1}, 
  {3, 3, 1,10, 1},
  {10, 10, 5, 8},
  {10, 10, 8, 5, 8}
};
short TimerPatterns [ NumPatterns][MaxPatternLength] = {
  {1, 1, 1, 2, 2, 1, 2, 2, 1, 1, 1, 1, 1},
  {1, 2, 2, 2, 4, 4, 4, 4, 1, 1, 2, 1, 1},
  {1, 1, 1, 1},
  {1, 1, 1, 2,2},
  {1, 1, 1, 1},
  {1, 1, 1, 2, 2}
};
short OctavePatterns [ NumPatterns][MaxPatternLength] = {
  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
  {1, 1, 2, 2},
  {1, 1, 2, 2,2},
  {1, 1, 2, 2},
  {1, 1, 2, 2, 2},
};
short VolumePatterns[ NumPatterns][MaxPatternLength] = {
  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
  {1, 1, 1, 1},
  {1, 1, 1, 1, 1},
  {1, 1, 1, 1},
  {1, 1, 1, 1, 1}
};

short encPos = Tempo;
short NewEncPos = Tempo;
short encPos2 = GateLength;
short NewEncPos2 = GateLength;




//float Range = 819.2; // (2^12/5)
float Range = 1365.33333; // (2^12/3)

volatile int hKey ;
volatile int hOctave;
volatile int hNote;
volatile int houtValue;

int CX = 0;
int CY = 0;
int Width = 8;
int Height = 8;


void MakeActivePattern(short Pattern) {
  int i = 0;
  CurrentPattern = Pattern;
  countlength = PatternLengths[Pattern];
  for (i = 0; i < MaxPatternLength; i++) {
    if (i < PatternLengths[Pattern]) {
      Notes[i] = NotePatterns[Pattern][i];
      Timers[i] = TimerPatterns[Pattern][i];
      Octave[i] = OctavePatterns[Pattern][i];
      Volume[i] = VolumePatterns[Pattern][i];
    }
  }
  CalculateBeatCountArray();
  BeatsNumber();
  DisplayBackground();
  DisplayTempo();
  DisplayMode();
  DisplayMode2();
  DisplayGateLength();
  DisplaySeqNum(CurrentPattern);

}


void BeatsNumber() {
  float tmp = 0.0;
  int i = 0;
  for (i = 0; i < countlength; i++) {
    tmp = tmp  + 1.0 / (float)Timers[i];
  }
  NumberOfBeats = tmp;
}
void CalculateBeatCountArray() {
  short TimerCount = 0;
  short BeatCount = 1;
  calcBeatPos[0] = 0;
  float CurrentBeatLength = 0;
  int countlength = PatternLengths[CurrentPattern];
  for (TimerCount = 1; TimerCount < countlength; TimerCount++) {

    CurrentBeatLength = CurrentBeatLength + (float) (1 / (float) Timers[TimerCount - 1]);

    if ((CurrentBeatLength == (float)round(CurrentBeatLength))) {
      calcBeatPos[BeatCount] = TimerCount;
      BeatCount++;

    }
  }
  MaxBeat = BeatCount;
  for (int i = 0; i < BeatCount; i++) {
    BeatPos[i] = calcBeatPos[i];
  }
}

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
  pinMode(DAC, OUTPUT);

  digitalWrite(DAC, HIGH);

}

void SetupSwitches() {
  pinMode(BUT1, INPUT);
  pinMode(BUT2, INPUT);
  pinMode(BUT3, INPUT);
  pinMode(BUTLED3, OUTPUT);
  pinMode(START, INPUT);
  pinMode(RESET, INPUT);
  attachInterrupt(BUT1, iMode2, FALLING);
  attachInterrupt(BUT2, iMode, FALLING);
  attachInterrupt(BUT3, interrupt, FALLING);

  attachInterrupt(START, StartInterrupt, RISING);

  attachInterrupt(RESET, IntReset, RISING);

  pinMode(Gate2, OUTPUT);
  pinMode(Gate1, OUTPUT);
}



void setup() {
  //These set the timer modes to be correct for accurate timing
  SequenceGateTimer.setChannel2Mode(TIMER_OUTPUT_COMPARE);
  SequenceGateTimer.setCompare(TIMER_CH2, 1);
  BeatTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  BeatTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  SequenceTimer.setChannel3Mode(TIMER_OUTPUT_COMPARE);
  SequenceTimer.setCompare(TIMER_CH3, 1);  // Interrupt 1 count after each update
  BeatGateTimer.setChannel4Mode(TIMER_OUTPUT_COMPARE);
  BeatGateTimer.setCompare(TIMER_CH4, 1); 
  
  ReadEEPROM();
  setTempo(Tempo);
  SetupOLED();
  MakeActivePattern(CurrentPattern);
  SetupEncoders();
  SetupDacs();
  SetupSwitches();
  
  
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
  digitalWrite(Gate1, true);
  long Time = (long)(BaseTime / Timers[count]);
  SequenceTimer.setPeriod(Time);
  
  SequenceTimer.refresh();
  SequenceTimer.resume();
  SequenceGateTimer.refresh();
  SequenceGateTimer.resume();

  //  Deal with notes

  WriteNote(Notes[count], Octave[count], CV1);

  // Display count
  DisplayCount(count + 1, 0, 0);

  //Display Cursor

  Cursor(9 * count, 12);
  DrawMode(INVERT);
  _WriteChar(113);

  if (ErasePos >= 0) {

    Cursor(9 * ErasePos, 12);
    DrawMode(INVERT);
    _WriteChar(113);
  }

  Refresh();
  ErasePos++;
  if (ErasePos >= countlength)
    ErasePos = 0;
  count++;
  if (count >= countlength) {
    count = 0;
    ErasePos = countlength - 1;
    if ((NextPattern >= 0) && (NextPattern < 50)) {
      MakeActivePattern(NextPattern);
      DisplaySeqNum(CurrentPattern);
      NextPattern = -1;
    }
  }

}


void BeatGateOn() {
  digitalWrite(Gate2, true);
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
  bErasePos = Beat - 1;
  if (bErasePos <= 0)
    bErasePos = MaxBeat;
  Cursor(9 * BeatPos[bErasePos - 1], 16);
  DrawMode(CLEAR);
  _WriteChar('_' - 32);
  Refresh();
  Beat++;
  if (Beat > MaxBeat)
    Beat = 1;


}

void BeatGateOff() {
  BeatGateTimer.pause();
  digitalWrite(Gate2, false);
  digitalWrite(BUTLED3, false);

}
void SequenceBeatGateOff() {
  SequenceGateTimer.pause();
  digitalWrite(Gate1, false);

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
  WriteEEProm();
}

boolean Running = false;
unsigned long TempoMicros = micros();
boolean Led=false;
unsigned long micro1=1000000;

void StartInterrupt(){
  interrupt();
  /*
  noInterrupts;
  
  unsigned long Now=micros();
  unsigned long Diff= Now-TempoMicros ;
  TempoMicros =Now;
  
  int dTempo=(int)floor(60L*micro1/Diff);
  DisplayDTempo(dTempo);
  Led=!Led;
  digitalWrite(BUTLED3, Led);
  interrupts;
  */
}

void interrupt() {
  
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

unsigned long currentMillis = millis();
unsigned long previousMillis = 0;
const long interval = 500;

void iMode2() {
  noInterrupts();
  currentMillis = millis();
  //DisplayTime(currentMillis,previousMillis);
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (Mode2 == 1) {

      NextPattern = encPos;
      encPos = Tempo;
      Mode2 = 0;

    } else {
      
      encPos = CurrentPattern;
      Mode2 = 1;
      
    }


    DisplayMode2();
  }
  interrupts();
}

void DisplayTime(unsigned long currentMillis,unsigned long previousMillis){
  int CX = 0 * 8;
  int CY = 5 * 8;
  Cursor(CX, CY);
  Erase(CX, CY, CX + Width * 15, CY + Height);
  char sTmp[6];
  sprintf(sTmp, " %i, %i", previousMillis,currentMillis);
  Print(sTmp);
  Refresh();
  
}

void DisplayGate(long GateTime){
  int CX = 0 * 8;
  int CY = 5 * 8;
  Cursor(CX, CY);
  Erase(CX, CY, CX + Width * 15, CY + Height);
  char sTmp[6];
  sprintf(sTmp, " %i", GateTime);
  Print(sTmp);
  Refresh();
  
}
void DisplayMode() {
  DisplayChar(Modes[Mode] - 32, 4, 0);
  Refresh();

}

void DisplayMode2() {
  DisplayChar(Modes2[Mode2] - 32, 6, 0);
  Refresh();

}


void IntReset() {
  count = 0;
}



void setTempo(int newTempo) {
  Tempo = newTempo;
  BaseTime = 1000000L * 60 / Tempo;
  BeatGateTime = 1000000L * 60 / Tempo;
  DisplayGate( BeatGateTime);
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
    encALast = encAVal;

  }
  return val;
}
void handleEnc1() {

  NewEncPos = encodeVal(encPos);
  if (NewEncPos != encPos) {
    switch (Mode2) {
      case 0: encPos = NewEncPos; //Tempo
        if (encPos < 1) {
          encPos = 1;
        }
        if (encPos > 240 ) {
          encPos = 240;
        }
        setTempo(encPos);
        DisplayTempo() ;
        break;
      case 1: // Pattern select
        encPos = NewEncPos;
        if (encPos < 1) {
          encPos = 0;
        }
        if (encPos >= NumPatterns ) {
          encPos = NumPatterns - 1;
        }
        DisplaySeqNum(encPos);

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
        GateLength = encPos2;
        SeqGateTime = BeatGateTime / GateLength;
        SequenceGateTimer.setPeriod(SeqGateTime);
        DisplayGateLength() ;
        break;
      default: break;
    }
  }
}

void DisplaySeqNum(short DisplayNum) {
  int CX = 0 * 8;
  int CY = 7 * 8;
  Cursor(CX, CY);
  Erase(CX, CY, CX + Width * 6, CY + Height);
  char sTmp[6];
  sprintf(sTmp, "Seq %i", DisplayNum);
  Print(sTmp);
  Refresh();
}

void DisplayBeatCount() {
  BeatsNumber();
  int CX = Width * 7;
  int CY = Height * 7;
  Cursor(CX, CY);
  Erase(CX, CY, CX + Width * 8 , CY + Height);
  char sTmp[6];
  sprintf(sTmp, "Beats %5.2f", NumberOfBeats);
  Print(sTmp);
  Refresh();

}


void DisplayChar(char C, byte x, byte y) {
  CX = Width * x;
  CY = Height * y;
  noInterrupts();
  Erase(CX, CY, CX + Width , CY + Height);
  Cursor(CX, CY);

  _WriteChar(C);
  interrupts();
}

//Display two digit integer
void DisplayCount(byte Count, byte x, byte y) {
  //Count++;

  byte low = 16 + Count % 10;
  byte high = 16 + Count / 10;
  if (high != 0) {
    DisplayChar(high, x, y);
  }
  DisplayChar(low, x + 1, y);
  Refresh();

}

void DisplayTempo() {
  int CX = 8 * 8;
  int CY = 0;
  Cursor(CX, CY);
  Erase(CX, CY, CX + Width * 3, CY + Height);
  Print(Tempo);
  Refresh();
}

void DisplayDTempo(int iTempo) {
  int CX = 8 * 8;
  int CY = 6*8;
  Cursor(CX, CY);
  Erase(CX, CY, CX + Width * 3, CY + Height);
  Print(iTempo);
  Refresh();
}

void DisplayGateLength() {
  CX = 12 * 8;
  Cursor(CX , CY);
  Erase(CX , CY, CX + Width , CY + Height);
  Print(9 - GateLength);
  Refresh();
}

void DisplayBackground() {
  ClearBuffer();
  Refresh();
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
  DisplayBeatCount();
}






void mcpWrite(int value, int Channel) {
  //CS
  noInterrupts();
  digitalWrite(DAC, LOW);
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
  digitalWrite(DAC, HIGH);
  interrupts();

}

void WriteNote(int Note, int Octave, int Channel) {
  hOctave = Octave;
  hNote = (byte)(Note % 12);
  houtValue = (int)(Range * (hOctave + (float)hNote / 12));


  mcpWrite(houtValue, Channel);

}


void ReadEEPROM() {
  int addr = 0;
  Tempo = EEPROM.read(addr);
  addr = addr + 1;
  GateLength = EEPROM.read(addr);
  addr = addr + 1;
  CurrentPattern = EEPROM.read(addr);
  if (CurrentPattern < 0)
    CurrentPattern = 0;
  encPos = Tempo;
  NewEncPos = Tempo;
  encPos2 = GateLength;
  NewEncPos2 = GateLength;
}

void WriteEEProm() {
  int addr = 0;
  EEPROM.write(addr, Tempo);
  addr = addr + 1;
  EEPROM.write(addr, GateLength);
  addr = addr + 1;
  EEPROM.write(addr, CurrentPattern);
}
