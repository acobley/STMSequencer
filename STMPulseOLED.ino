


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
#include "Setup.h"
/*
void BeatsNumber() {
  float tmp = 0.0;
  int i = 0;
  for (i = 0; i < countlength; i++) {
    if (Timers[i] >= 3) {
      tmp = tmp  + 1.0 / (float)NoteLengths[Timers[i]];
    } else {
      if (Timers[i] == 1) {
        tmp = tmp  + 4;
      } else {
        if (Timers[i] == 2) {
          tmp = tmp  + 2;
        }
      }
    }
  }
  NumberOfBeats = tmp;
}
*/

void BeatsNumber() {
  float tmp = 0.0;
  int i = 0;
  for (i = 0; i < countlength; i++) {
   
      tmp = tmp  +  (float)BeatLengths[Timers[i]];
    
  }
  NumberOfBeats = tmp;
}
#include "Display.h"
#include <EEPROM.h>



short PatternLengths[NumPatterns] = {13, 13, 4, 5, 4, 5, 4};
short NotePatterns [ NumPatterns][MaxPatternLength] = {

  {10, 8, 1, 3, 8, 5, 3, 8, 5, 1, 3, 10, 8},
  {10, 8, 1, 3, 8, 5, 3, 8, 5, 1, 3, 10, 8},
  {3, 3, 10, 1},
  {3, 3, 1, 10, 1},
  {10, 10, 5, 8},
  {10, 10, 8, 5, 8},
  {3, 3, 10, 1},
};
short TimerPatterns [ NumPatterns][MaxPatternLength] = {

  {3, 3, 3, 4, 4, 3, 4, 4, 3, 3, 3, 3, 3},
  {3, 4, 4, 4, 5, 5, 5, 5, 3, 3, 4, 3, 3},
  {3, 3, 3, 3},
  {2, 2, 2, 3, 3},
  {3, 3, 3, 3},
  {2, 2, 2, 3, 3},
  {1, 1, 1, 1},
};
short OctavePatterns [ NumPatterns][MaxPatternLength] = {

  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
  {1, 1, 2, 2},
  {1, 1, 2, 2, 2},
  {1, 1, 2, 2},
  {1, 1, 2, 2, 2},
  {1, 1, 2, 2},
};
short TripletPatterns[ NumPatterns][MaxPatternLength] = {

  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0},
};

short DottedPatterns[ NumPatterns][MaxPatternLength] = {

  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0},
};

short VolumePatterns[ NumPatterns][MaxPatternLength] = {

  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
  {1, 1, 1, 1},
  {1, 1, 1, 1, 1},
  {1, 1, 1, 1},
  {1, 1, 1, 1, 1},
  {1, 1, 1, 1},
};



//Low level routines to handle the timers.
//See https://www.st.com/content/ccc/resource/training/technical/product_training/c4/1b/56/83/3a/a1/47/64/STM32L4_WDG_TIMERS_GPTIM.pdf/files/STM32L4_WDG_TIMERS_GPTIM.pdf/jcr:content/translations/en.STM32L4_WDG_TIMERS_GPTIM.pdf
//Page 31 and 32

double Div = 1 / 72.0;
double PWM1 = 1;
double PWM2 = 1;
void CalcTPeriod(ClockStruct *Clock, long Period, HardwareTimer myTimer, PWMStruct *PWMS) {

  Clock->Psc = 1;
  Clock->Arr = 0;
  Clock->Arr = Period / (Div * Clock->Psc);
  while (Clock->Arr >= 65535) {
    Clock->Psc = Clock->Psc + 1;
    Clock->Arr = Period / (Div * Clock->Psc);

  }
  myTimer.setPrescaleFactor(Clock->Psc);
  myTimer.setOverflow(Clock->Arr);

  PWMS->PWM1 = (double)((double)Period / (double)15000L);
  PWMS->PWM2 = (double)((double)Period / (double)(Period - 15000L));
  //myTimer.setCompare(TIMER_CH1, Clock->Arr / PWMS->PWM1);
  myTimer.refresh();

  //DisplayTime(Period,4);
  //DisplayTime(Clock->Psc,Clock->Arr,5);
  //DisplayTime(Clock->Arr / PWMS->PWM1, Clock->Arr / PWMS->PWM2,6);

}

void CalcTPeriod(ClockStruct *Clock, long Period, HardwareTimer myTimer, PWMStruct *PWMS, long onTime) {

  Clock->Psc = 1;
  Clock->Arr = 0;
  Clock->Arr = Period / (Div * Clock->Psc);
  while (Clock->Arr >= 65535) {
    Clock->Psc = Clock->Psc + 1;
    Clock->Arr = Period / (Div * Clock->Psc);

  }
  myTimer.setPrescaleFactor(Clock->Psc);
  myTimer.setOverflow(Clock->Arr);

  PWMS->PWM1 = (double)((double)Period / (double)onTime);
  PWMS->PWM2 = (double)((double)Period / (double)(Period - onTime));
  //myTimer.setCompare(TIMER_CH1, Clock->Arr / PWMS->PWM1);
  myTimer.refresh();

  DisplayTime(Period, 4);
  DisplayTime(Clock->Psc, Clock->Arr, 5);
  DisplayTime(Clock->Arr / PWMS->PWM1, Clock->Arr / PWMS->PWM2, 6);

}

void CalcRatios(ClockStruct *Clock, long Period, PWMStruct *PWMS, long onTime) {

  PWMS->PWM1 = (double)((double)Period / (double)onTime);
  PWMS->PWM2 = (double)((double)Period / (double)(Period - onTime));
  //myTimer.setCompare(TIMER_CH1, Clock->Arr / PWMS->PWM1);
  DisplayTime(Period, onTime, 4);
  DisplayTime(Clock->Psc, Clock->Arr, 5);
  DisplayTime(Clock->Arr / PWMS->PWM1, Clock->Arr / PWMS->PWM2, 6);

}

void SetupSwitches() {
  pinMode(BUT1, INPUT);
  pinMode(BUT2, INPUT);
  pinMode(STARTBUT, INPUT);
  pinMode(BUTLED3, OUTPUT);
  pinMode(START, INPUT);
  pinMode(RESET, INPUT);
  attachInterrupt(BUT1, iMode2, FALLING);
  attachInterrupt(BUT2, iMode, FALLING);
  attachInterrupt(STARTBUT, StartButtonInterrupt, FALLING);
  attachInterrupt(START, StartInterrupt, RISING);
  attachInterrupt(RESET, IntReset, RISING);

}

void SetupTimers() {
  //These set the timer modes to be correct for accurate timing
  // See http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/latest/lang/api/hardwaretimer.html

  SetUpTimer(SequenceTimer, SequenceGateOn, TIMER_OUTPUT_COMPARE);
  SetUpTimer(BeatTimer, BeatGate, TIMER_OUTPUT_COMPARE);

}

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
  DisplaySeqNum();

  DisplayTempo();
  DisplayMode();
  DisplayMode2();
  DisplayGateLength();


}




void CalculateBeatCountArray() {
  short TimerCount = 0;
  short BeatCount = 1;
  calcBeatPos[0] = 0;
  float CurrentBeatLength = 0;
  int countlength = PatternLengths[CurrentPattern];
  for (TimerCount = 1; TimerCount < countlength; TimerCount++) {

    CurrentBeatLength = CurrentBeatLength + (float)BeatLengths[Timers[TimerCount]];

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



void setup() {
  //flash(5);
  SetupOLED();
  ReadEEPROM();
  SetupTimers();
  setTempo(Tempo);
  MakeActivePattern(CurrentPattern);
  SetupEncoders();
  SetupDacs();
  SetupSwitches();
  SetupGates();
  digitalWrite(BUTLED3, false);


}


void loop() {
  handleEnc1();
  handleEnc2();
}

void StartAllTimers() {

  BeatTimer.resume();
  SequenceTimer.resume();
  SequenceGateOn();

}

boolean SequenceState = true;

int lastLength = NoteLengths[Timers[PlaceCount]];
void SequenceGateOn() {
  if (SequenceState == true) {
    digitalWrite(Gate1, true);
    //  Deal with notes
    WriteNote(Notes[PlaceCount], Octave[PlaceCount], CV1);

    // Display count
    DisplayCount(PlaceCount + 1, 0, 0);

    long Time = (long)(BaseTime / NoteLengths[Timers[PlaceCount]]);
    //DisplayCount(NoteLengths[Timers[PlaceCount]], 3,0);
    SeqGateTime = (long)((double)Time * ((double)GateLength / 8.0));
    CalcRatios(&SequenceClock, Time, &SequencePWMS, SeqGateTime);
    lastLength = NoteLengths[Timers[PlaceCount]];
    SequenceTimer.setCompare(TIMER_CH1, (SequenceClock.Arr / lastLength) / SequencePWMS.PWM1);
    SequenceTimer.setCount(0);
    SequenceTimer.refresh();

    //Display Cursor

    Cursor(9 * PlaceCount, 12);
    DrawMode(INVERT);
    _WriteChar(BLOCK);

    if (ErasePos >= 0) {

      Cursor(9 * ErasePos, 12);
      DrawMode(INVERT);
      _WriteChar(BLOCK);
    }

    Refresh();
    ErasePos++;
    if (ErasePos >= countlength)
      ErasePos = 0;
    PlaceCount++;
    if (PlaceCount >= countlength) {
      PlaceCount = 0;
      ErasePos = countlength - 1;
      if ((NextPattern >= 0) && (NextPattern < 50)) {
        MakeActivePattern(NextPattern);
        DisplaySeqNum(CurrentPattern);
        NextPattern = -1;
      }
    }
  } else {
    digitalWrite(Gate1, false);
    SequenceTimer.setCompare(TIMER_CH1, (SequenceClock.Arr / lastLength) / SequencePWMS.PWM2);
    SequenceTimer.setCount(0);
    SequenceTimer.refresh();
  }
  SequenceState = !SequenceState;
}

boolean BeatState = false;
void BeatGate(void) {
  BeatTime = 1000000L * 60 / Tempo;

  if (BeatState == true) {
    digitalWrite(Gate2, HIGH);
    digitalWrite(BUTLED3, HIGH);
    BeatTimer.setCompare(TIMER_CH1, BeatClock.Arr / BeatPWMS.PWM1);
    BeatTimer.refresh();

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

  } else {
    digitalWrite(Gate2, LOW);
    digitalWrite(BUTLED3, LOW);
    BeatTimer.setCompare(TIMER_CH1, BeatClock.Arr / BeatPWMS.PWM2);
    BeatTimer.refresh();
  }
  BeatState = !BeatState;



}


boolean Running = false;
unsigned long TempoMicros = micros();
boolean Led = false;
unsigned long micro1 = 1000000;

void StartInterrupt() {
  StartButtonInterrupt();

}

void StartButtonInterrupt() {
  digitalWrite(BUTLED3, !digitalRead(BUTLED3));
  Running = !Running; //invert state

  if (Running) {
    Start();
  } else {
    Stop();
  }

}

void Start() {
  StartAllTimers();
}

void Stop() {
  digitalWrite(Gate1, false); //Avoid stuck notes
  digitalWrite(Gate2, false);
  SequenceTimer.setCount(0);
  SequenceTimer.pause();
  BeatTimer.setCount(0);
  BeatTimer.pause();
  WriteEEProm();
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


int SencPos= CurrentPattern;

void iMode2() {
  noInterrupts();
  currentMillis = millis();
  //DisplayTime(currentMillis,previousMillis);
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (Mode2 == 1) {
      NextPattern = SencPos;
      //encPos = TencPos;
       DisplaySeqNum(NewEncPos);
       DisplayTempo();
      Mode2 = 0;

    } else { //From 0 to 1
      //TencPos=encPos;
      SencPos = CurrentPattern;
      Mode2 = 1;
    }
    DisplayMode2();
  }
  interrupts();
}




void handleEnc1() {
  switch (Mode2) {
    
    case 0:
      NewEncPos = encodeVal(Tempo);
      if (NewEncPos != Tempo) {
        if (NewEncPos < MinTempo) {
            NewEncPos = MinTempo;
          }
          if (NewEncPos > MaxTempo ) {
            NewEncPos = MaxTempo;
          }
          setTempo(NewEncPos);
      }
      break;
    case 1:
    NewEncPos = encodeVal(SencPos);
      if (NewEncPos != SencPos) {
        if (NewEncPos < 1) {
            NewEncPos = 0;
          }
          if (NewEncPos >= NumPatterns ) {
            NewEncPos = NumPatterns - 1;
          }
          DisplaySeqNum(NewEncPos);
          SencPos=NewEncPos;  // will be set to next pattern when mode changes
      }
      break;
    default: break;
  }

  
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


void IntReset() {
  PlaceCount = 0;
}



void setTempo(int newTempo) {
  Tempo = newTempo;
  BaseTime = 1000000L * (4 * 60) / Tempo;
  BeatTime = 1000000L * 60 / Tempo;
  //DisplayTime (BeatTime );
  CalcTPeriod(&BeatClock, BeatTime / 2, BeatTimer, &BeatPWMS);
  CalcTPeriod(&SequenceClock, BaseTime, SequenceTimer, &SequencePWMS, SeqGateTime);
  DisplayTempo() ;
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
        //SeqGateTime = Time * (GateLength / 8);
        //CalcTPeriod(&SequenceClock, BaseTime, SequenceTimer, &SequencePWMS, SeqGateTime);
        DisplayGateLength() ;
        break;
      default: break;
    }
  }
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
  if ((Tempo < 10) || (Tempo > 300))
    Tempo = DefaultTempo;
  setTempo(Tempo);
  addr = addr + 1;
  GateLength = EEPROM.read(addr);
  if (GateLength < 1)
    GateLength = 4;
  if (GateLength > 8)
    GateLength = 4;
  addr = addr + 1;
  CurrentPattern = EEPROM.read(addr);
  if (CurrentPattern < 0)
    CurrentPattern = 2;
  if (CurrentPattern >= NumPatterns)
    CurrentPattern = NumPatterns - 1;
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
