
void DisplayChar(char C, byte x, byte y) {
  CX = Width * x;
  CY = Height * y;
  noInterrupts();
  Erase(CX, CY, CX + Width , CY + Height);
  Cursor(CX, CY);

  _WriteChar(C);
  interrupts();
}

void DisplayTime(unsigned long currentMillis, unsigned long previousMillis, int Row) {
  return; // Remove for Debug
  int CX = 0 * 8;
  int CY = Row * 8;
  Cursor(CX, CY);
  Erase(CX, CY, CX + Width * 15, CY + Height);
  char sTmp[6];
  sprintf(sTmp, "%i,%i", currentMillis, previousMillis);
  Print(sTmp);
  Refresh();

}

void DisplayTime(long Time, int Row) {
  return; // Remove for Debug
  int CX = 0 * 8;
  int CY = Row * 8;
  Cursor(CX, CY);
  Erase(CX, CY, CX + Width * 15, CY + Height);
  char sTmp[6];
  sprintf(sTmp, "T %i", Time);
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

void DisplaySeqNum() {

  int CX = 0 * 8;
  int CY = 7 * 8;
  Cursor(CX, CY);
  Erase(CX, CY, CX + Width * 6, CY + Height);
  char sTmp[6];
  sprintf(sTmp, "Seq %i", CurrentPattern);
  Print(sTmp);
  Refresh();
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
  int CY = 6 * 8;
  Cursor(CX, CY);
  Erase(CX, CY, CX + Width * 3, CY + Height);
  Print(iTempo);
  Refresh();
}

void DisplayGateLength() {
  CX = 12 * 8;
  Cursor(CX , CY);
  Erase(CX , CY, CX + Width , CY + Height);
  Print(GateLength);
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
    //_WriteChar('~' - 32 + Timers[i]);
    _WriteChar((char)(cNoteLengthStart + Timers[i]));
  }

  for (i = 0; i < countlength; i++) {
    Cursor(i * 9, 24);
    //_WriteChar('~' - 32 + 7 + Notes[i]);
    _WriteChar((char)(cNoteStart + Notes[i]));
  }

  for (i = 0; i < countlength; i++) {
    Cursor(i * 9, 33);
    _WriteChar('0' - 32 + Octave[i]);
  }
  DisplayBeatCount();
}
