
#include "SPI.h"
#include "fonts.h"


//Encoder
#define encA PB0
#define encB PB1

#define enc2A PB10 //11
#define enc2B PB11 //10
#define BUT1 PA0
#define BUT2 PB15
#define STARTBUT PB7
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

HardwareTimer SequenceTimer(2);

HardwareTimer BeatTimer(4);


const short MaxPatternLength = 16;
short Timers[MaxPatternLength] ;
short calcBeatPos[MaxPatternLength];
short BeatPos[MaxPatternLength] ;
short Notes[MaxPatternLength] ;
short Octave[MaxPatternLength] ;
short Volume[MaxPatternLength] ;
int countlength = sizeof(Notes) / sizeof(short);

const short NoteLengths[6] = {0, 1, 2, 4, 8, 16};
const float BeatLengths[6] = {0, 4, 2, 1, 0.5, 0.25};
const short NumPatterns = 7;

long ClockF = 72000000;
struct ClockStruct {
  char Name[15];
  int Psc = 0;
  int Arr = 0;
};

struct PWMStruct {
  double PWM1 = 1;
  double PWM2 = 1;
};

PWMStruct BeatPWMS;
ClockStruct BeatClock;
PWMStruct SequencePWMS;
ClockStruct SequenceClock;


const int DefaultTempo=120;
int Tempo = DefaultTempo;
long BaseTime = 1000000L * (4 * 60) / Tempo;
long BeatTime = 1000000L * 60 / Tempo;
short GateLength = 4;
long SeqGateTime = BaseTime * (GateLength / 8);
long ms15 = 015000L;
const short MinTempo = 30;
const short MaxTempo = 300;

int PlaceCount = 0;
int ErasePos = -1;
int bErasePos;
int Beat = 1;
int MaxBeat = 10;

float NumberOfBeats = 1.0;
short CurrentPattern = 0;
short NextPattern = -1;




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



void SetupGates() {
  pinMode(Gate2, OUTPUT);
  pinMode(Gate1, OUTPUT);
}

void SetUpTimer(HardwareTimer myTimer, voidFuncPtr handler, timer_mode tm) {
  myTimer.pause();
  myTimer.setMode(TIMER_CH1, tm);
  myTimer.attachInterrupt(TIMER_CH1, handler);

}
