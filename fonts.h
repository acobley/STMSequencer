enum HCuOLEDDrawModes
{
  NORMAL    = 1,
  INVERT    = 2,
  CLEAR     = 3,
  OUTLINE   = 4,
  SOLID     = 5

};

#define CS_DI PB12
/* Digital pin number for the displays data/command pin */
#define DC_DI PB13
/* Digital pin number for the displays reset pin */
#define RST_DI PB14

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




#define BUFFERCOLSIZE 128
#define BUFFERROWSIZE 8

/* Display command register addresses */
#define COLLOWADD 0x00
#define COLHIGHADD 0x10
#define MEMORYADDRESSMODE 0x20
#define SETCOLADDRESS 0x21
#define SETPAGEADDRESS 0x22
#define CHARGEPUMP 0x8D
#define SETMUXRATIO 0xA8
#define DISPLAYONADD 0xAE
#define PAGEADD 0xB0
#define STARTLINEADD 0x40
#define SCANDIRECTIONADD 0xC0
#define SEGMENTMAPADD 0xA0

/* Command register parameters */
#define SCANDIRNORMAL 0x00
#define SCANDIRREVERSE 0x08
#define SEGMENTMAPNORMAL 0
#define SEGMENTMAPREVERSE 1
#define ENABLECHARGEPUMP 0x14
#define HORIZONTALADDRESSMODE 0x00
#define SH1106_RES_X 128
#define SH1106_RES_Y 64
#define SH1106_GRAM_COL_START 0x02
#define SH1106_GRAM_COL_END 0x7F
#define SH1106_GRAM_PAGE_START 0
#define SH1106_GRAM_PAGE_END 7



static byte _DrawMode;
byte _Res_Max_X;
byte _Res_Max_Y;
byte _GRAM_Col_Start;
byte _GRAM_Col_End;
byte _GRAM_Page_Start;
byte _GRAM_Page_End;
byte _RAM_Pages;
byte _DisplayType;

boolean _V_Ori;
boolean _H_Ori;

byte DisplayBuffer[BUFFERCOLSIZE][8];

static const byte *_FontType;
static const unsigned int *_FontDescriptor;
static byte _FontHight;

typedef struct _font_info
{
  byte CharacterHeight;
  char StartCharacter;
  char EndCharacter;
  const unsigned int *Descriptors;
  const byte *Bitmaps;
  //int Bitmaps;
} FONT_INFO;



/* Character bitmaps for small fixed width font (Terminal_8pt) */
const PROGMEM byte Terminal_8pt[] =
{
  // @0 ' ' (8 pixels wide)
  //
  //
  //
  //
  //
  //
  //
  //
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  // @8 '!' (2 pixels wide)
  // ##
  // ##
  // ##
  // ##
  //
  //
  // ##
  //
  0x4F, 0x4F,

  // @10 '"' (5 pixels wide)
  // ## ##
  // ## ##
  // ## ##
  //
  //
  //
  //
  //
  0x07, 0x07, 0x00, 0x07, 0x07,

  // @15 '#' (7 pixels wide)
  //  ## ##
  //  ## ##
  // #######
  //  ## ##
  // #######
  //  ## ##
  //  ## ##
  //
  0x14, 0x7F, 0x7F, 0x14, 0x7F, 0x7F, 0x14,

  // @22 '$' (6 pixels wide)
  //   ##
  //  #####
  // ####
  //  ####
  //   ####
  // #####
  //   ##
  //
  0x24, 0x2E, 0x7F, 0x7F, 0x3A, 0x12,

  // @28 '%' (7 pixels wide)
  //
  // ###  ##
  // ### ##
  //    ##
  //   ##
  //  ## ###
  // ##  ###
  //
  0x46, 0x66, 0x36, 0x18, 0x6C, 0x66, 0x62,

  // @35 '&' (7 pixels wide)
  //  ###
  // ## ##
  // ## ##
  //  ###
  // ## ####
  // ##  ##
  //  ######
  //
  0x36, 0x7F, 0x49, 0x5F, 0x76, 0x70, 0x50,

  // @42 ''' (4 pixels wide)
  //   ##
  //  ##
  // ##
  //
  //
  //
  //
  //
  0x04, 0x06, 0x03, 0x01,

  // @46 '(' (4 pixels wide)
  //   ##
  //  ##
  // ##
  // ##
  // ##
  //  ##
  //   ##
  //
  0x1C, 0x3E, 0x63, 0x41,

  // @50 ')' (4 pixels wide)
  // ##
  //  ##
  //   ##
  //   ##
  //   ##
  //  ##
  // ##
  //
  0x41, 0x63, 0x3E, 0x1C,

  // @54 '*' (6 pixels wide)
  //   ##
  // ######
  //  ####
  // ######
  //  ####
  // ######
  //   ##
  //
  0x2A, 0x3E, 0x7F, 0x7F, 0x3E, 0x2A,

  // @60 '+' (6 pixels wide)
  //
  //   ##
  //   ##
  // ######
  //   ##
  //   ##
  //
  //
  0x08, 0x08, 0x3E, 0x3E, 0x08, 0x08,

  // @66 ',' (3 pixels wide)
  //
  //
  //
  //
  //
  //  ##
  //  ##
  // ##
  0x80, 0xE0, 0x60,

  // @69 '-' (7 pixels wide)
  //
  //
  //
  // #######
  //
  //
  //
  //
  0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,

  // @76 '.' (3 pixels wide)
  //
  //
  //
  //
  //
  // ###
  // ###
  //
  0x60, 0x60, 0x60,

  // @79 '/' (7 pixels wide)
  //
  //      ##
  //     ##
  //    ##
  //   ##
  //  ##
  // ##
  //
  0x40, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x02,

  // @86 '0' (7 pixels wide)
  //  #####
  // ##   ##
  // ##  ###
  // #######
  // ###  ##
  // ##   ##
  //  #####
  //
  0x3E, 0x7F, 0x59, 0x49, 0x4D, 0x7F, 0x3E,

  // @93 '1' (6 pixels wide)
  //   ##
  //  ###
  // ####
  //   ##
  //   ##
  //   ##
  // ######
  //
  0x44, 0x46, 0x7F, 0x7F, 0x40, 0x40,

  // @99 '2' (7 pixels wide)
  //  #####
  // ##   ##
  //      ##
  //    ###
  //  ###
  // ##
  // #######
  //
  0x62, 0x73, 0x51, 0x59, 0x49, 0x4F, 0x46,

  // @106 '3' (7 pixels wide)
  //  #####
  // ##   ##
  //      ##
  //   ####
  //      ##
  // ##   ##
  //  #####
  //
  0x22, 0x63, 0x49, 0x49, 0x49, 0x7F, 0x36,

  // @113 '4' (7 pixels wide)
  //     ##
  //    ###
  //   ####
  //  ## ##
  // #######
  //     ##
  //     ##
  //
  0x10, 0x18, 0x1C, 0x16, 0x7F, 0x7F, 0x10,

  // @120 '5' (7 pixels wide)
  // #######
  // ##
  // #####
  //     ##
  //      ##
  // ##  ##
  //  ####
  //
  0x27, 0x67, 0x45, 0x45, 0x6D, 0x39, 0x11,

  // @127 '6' (7 pixels wide)
  //   ####
  //  ##
  // ##
  // ######
  // ##   ##
  // ##   ##
  //  #####
  //
  0x3C, 0x7E, 0x4B, 0x49, 0x49, 0x79, 0x30,

  // @134 '7' (7 pixels wide)
  // #######
  // ##   ##
  //     ##
  //    ##
  //   ##
  //   ##
  //   ##
  //
  0x03, 0x03, 0x71, 0x79, 0x0D, 0x07, 0x03,

  // @141 '8' (7 pixels wide)
  //  #####
  // ##   ##
  // ##   ##
  //  #####
  // ##   ##
  // ##   ##
  //  #####
  //
  0x36, 0x7F, 0x49, 0x49, 0x49, 0x7F, 0x36,

  // @148 '9' (7 pixels wide)
  //  #####
  // ##   ##
  // ##   ##
  //  ######
  //      ##
  //     ##
  //  ####
  //
  0x06, 0x4F, 0x49, 0x49, 0x69, 0x3F, 0x1E,

  // @155 ':' (2 pixels wide)
  //
  //
  // ##
  //
  //
  // ##
  //
  //
  0x24, 0x24,

  // @157 ';' (3 pixels wide)
  //
  //
  //  ##
  //
  //
  //  ##
  //  ##
  // ##
  0x80, 0xE4, 0x64,

  // @160 '<' (7 pixels wide)
  //    ####
  //   ###
  //  ###
  // ###
  //  ###
  //   ###
  //    ####
  //
  0x08, 0x1C, 0x3E, 0x77, 0x63, 0x41, 0x41,

  // @167 '=' (7 pixels wide)
  //
  //
  // #######
  //
  // #######
  //
  //
  //
  0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14,

  // @174 '>' (7 pixels wide)
  // ####
  //   ###
  //    ###
  //     ###
  //    ###
  //   ###
  // ####
  //
  0x41, 0x41, 0x63, 0x77, 0x3E, 0x1C, 0x08,

  // @181 '?' (7 pixels wide)
  //  #####
  // ##   ##
  //      ##
  //    ###
  //   ##
  //
  //   ##
  //
  0x02, 0x03, 0x51, 0x59, 0x09, 0x0F, 0x06,

  // @188 '@' (7 pixels wide)
  //   ####
  //  ##  ##
  // ## ####
  // #######
  // ## ###
  //  ##
  //   #####
  //
  0x1C, 0x3E, 0x6B, 0x5D, 0x5D, 0x5F, 0x4E,

  // @195 'A' (7 pixels wide)
  //   ###
  //  ## ##
  // ##   ##
  // #######
  // ##   ##
  // ##   ##
  // ##   ##
  //
  0x7C, 0x7E, 0x0B, 0x09, 0x0B, 0x7E, 0x7C,

  // @202 'B' (7 pixels wide)
  // ######
  //  ##  ##
  //  ##  ##
  //  #####
  //  ##  ##
  //  ##  ##
  // ######
  //
  0x41, 0x7F, 0x7F, 0x49, 0x49, 0x7F, 0x36,

  // @209 'C' (7 pixels wide)
  //   ####
  //  ##  ##
  // ##
  // ##
  // ##
  //  ##  ##
  //   ####
  //
  0x1C, 0x3E, 0x63, 0x41, 0x41, 0x63, 0x22,

  // @216 'D' (7 pixels wide)
  // #####
  //  ## ##
  //  ##  ##
  //  ##  ##
  //  ##  ##
  //  ## ##
  // #####
  //
  0x41, 0x7F, 0x7F, 0x41, 0x63, 0x3E, 0x1C,

  // @223 'E' (7 pixels wide)
  // #######
  // ##
  // ##
  // #####
  // ##
  // ##
  // #######
  //
  0x7F, 0x7F, 0x49, 0x49, 0x49, 0x41, 0x41,

  // @230 'F' (7 pixels wide)
  // #######
  // ##
  // ##
  // #####
  // ##
  // ##
  // ##
  //
  0x7F, 0x7F, 0x09, 0x09, 0x09, 0x01, 0x01,

  // @237 'G' (7 pixels wide)
  //   ####
  //  ##  ##
  // ##
  // ## ####
  // ##   ##
  //  ##  ##
  //   ####
  //
  0x1C, 0x3E, 0x63, 0x49, 0x49, 0x7B, 0x3A,

  // @244 'H' (7 pixels wide)
  // ##   ##
  // ##   ##
  // ##   ##
  // #######
  // ##   ##
  // ##   ##
  // ##   ##
  //
  0x7F, 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x7F,

  // @251 'I' (4 pixels wide)
  // ####
  //  ##
  //  ##
  //  ##
  //  ##
  //  ##
  // ####
  //
  0x41, 0x7F, 0x7F, 0x41,

  // @255 'J' (7 pixels wide)
  //    ####
  //     ##
  //     ##
  //     ##
  //     ##
  // ##  ##
  //  ####
  //
  0x20, 0x60, 0x40, 0x41, 0x7F, 0x3F, 0x01,

  // @262 'K' (7 pixels wide)
  // ##   ##
  // ##  ##
  // ## ##
  // ####
  // ## ##
  // ##  ##
  // ##   ##
  //
  0x7F, 0x7F, 0x08, 0x1C, 0x36, 0x63, 0x41,

  // @269 'L' (7 pixels wide)
  // ##
  // ##
  // ##
  // ##
  // ##
  // ##
  // #######
  //
  0x7F, 0x7F, 0x40, 0x40, 0x40, 0x40, 0x40,

  // @276 'M' (7 pixels wide)
  // ##   ##
  // ### ###
  // #######
  // #######
  // ##   ##
  // ##   ##
  // ##   ##
  //
  0x7F, 0x7F, 0x0E, 0x0C, 0x0E, 0x7F, 0x7F,

  // @283 'N' (7 pixels wide)
  // ##   ##
  // ###  ##
  // #### ##
  // ## ####
  // ##  ###
  // ##   ##
  // ##   ##
  //
  0x7F, 0x7F, 0x06, 0x0C, 0x18, 0x7F, 0x7F,

  // @290 'O' (7 pixels wide)
  //   ###
  //  ## ##
  // ##   ##
  // ##   ##
  // ##   ##
  //  ## ##
  //   ###
  //
  0x1C, 0x3E, 0x63, 0x41, 0x63, 0x3E, 0x1C,

  // @297 'P' (7 pixels wide)
  // ######
  // ##   ##
  // ##   ##
  // ######
  // ##
  // ##
  // ##
  //
  0x7F, 0x7F, 0x09, 0x09, 0x09, 0x0F, 0x06,

  // @304 'Q' (7 pixels wide)
  //   ###
  //  ## ##
  // ##   ##
  // ##   ##
  // ## ####
  //  ## ##
  //   #####
  //
  0x1C, 0x3E, 0x63, 0x51, 0x73, 0x7E, 0x5C,

  // @311 'R' (7 pixels wide)
  // ######
  // ##   ##
  // ##   ##
  // ######
  // ## ##
  // ##  ##
  // ##   ##
  //
  0x7F, 0x7F, 0x09, 0x19, 0x39, 0x6F, 0x46,

  // @318 'S' (7 pixels wide)
  //  #####
  // ##   ##
  // ##
  //  #####
  //      ##
  // ##   ##
  //  #####
  //
  0x26, 0x6F, 0x49, 0x49, 0x49, 0x7B, 0x32,

  // @325 'T' (6 pixels wide)
  // ######
  //   ##
  //   ##
  //   ##
  //   ##
  //   ##
  //   ##
  //
  0x01, 0x01, 0x7F, 0x7F, 0x01, 0x01,

  // @331 'U' (7 pixels wide)
  // ##   ##
  // ##   ##
  // ##   ##
  // ##   ##
  // ##   ##
  // ##   ##
  //  #####
  //
  0x3F, 0x7F, 0x40, 0x40, 0x40, 0x7F, 0x3F,

  // @338 'V' (7 pixels wide)
  // ##   ##
  // ##   ##
  // ##   ##
  //  ## ##
  //  ## ##
  //   ###
  //   ###
  //
  0x07, 0x1F, 0x78, 0x60, 0x78, 0x1F, 0x07,

  // @345 'W' (7 pixels wide)
  // ##   ##
  // ##   ##
  // ##   ##
  // #######
  // #######
  // ### ###
  // ##   ##
  //
  0x7F, 0x7F, 0x38, 0x18, 0x38, 0x7F, 0x7F,

  // @352 'X' (7 pixels wide)
  // ##   ##
  // ##   ##
  //  ## ##
  //   ###
  //  ## ##
  // ##   ##
  // ##   ##
  //
  0x63, 0x77, 0x1C, 0x08, 0x1C, 0x77, 0x63,

  // @359 'Y' (6 pixels wide)
  // ##  ##
  // ##  ##
  // ##  ##
  //  ####
  //   ##
  //   ##
  //   ##
  //
  0x07, 0x0F, 0x78, 0x78, 0x0F, 0x07,

  // @365 'Z' (7 pixels wide)
  // #######
  //      ##
  //     ##
  //   ###
  //  ##
  // ##
  // #######
  //
  0x61, 0x71, 0x59, 0x49, 0x4D, 0x47, 0x43,

  // @372 '[' (5 pixels wide)
  // #####
  // ##
  // ##
  // ##
  // ##
  // ##
  // #####
  //
  0x7F, 0x7F, 0x41, 0x41, 0x41,

  // @377 '\' (7 pixels wide)
  //
  // ##
  //  ##
  //   ##
  //    ##
  //     ##
  //      ##
  //
  0x02, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40,

  // @384 ']' (5 pixels wide)
  // #####
  //    ##
  //    ##
  //    ##
  //    ##
  //    ##
  // #####
  //
  0x41, 0x41, 0x41, 0x7F, 0x7F,

  // @389 '^' (6 pixels wide)
  //
  //   ##
  //  ####
  // ##  ##
  //
  //
  //
  //
  0x08, 0x0C, 0x06, 0x06, 0x0C, 0x08,

  // @395 '_' (9 pixels wide)
  //
  //
  //
  //
  //
  //
  //
  // #########
  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,

  // @404 '`' (4 pixels wide)
  // ##
  //  ##
  //   ##
  //
  //
  //
  //
  //
  0x01, 0x03, 0x06, 0x04,

  // @408 'a' (7 pixels wide)
  //
  //
  //  ####
  //     ##
  //  #####
  // ##  ##
  //  ######
  //
  0x20, 0x74, 0x54, 0x54, 0x7C, 0x78, 0x40,

  // @415 'b' (7 pixels wide)
  // ##
  // ##
  // ######
  // ###  ##
  // ##   ##
  // ###  ##
  // ######
  //
  0x7F, 0x7F, 0x6C, 0x44, 0x44, 0x7C, 0x38,

  // @422 'c' (7 pixels wide)
  //
  //
  //  #####
  // ##   ##
  // ##
  // ##   ##
  //  #####
  //
  0x38, 0x7C, 0x44, 0x44, 0x44, 0x6C, 0x28,

  // @429 'd' (7 pixels wide)
  //      ##
  //      ##
  //  ######
  // ##  ###
  // ##   ##
  // ##  ###
  //  ######
  //
  0x38, 0x7C, 0x44, 0x44, 0x6C, 0x7F, 0x7F,

  // @436 'e' (7 pixels wide)
  //
  //
  //  #####
  // ##   ##
  // #######
  // ##
  //  #####
  //
  0x38, 0x7C, 0x54, 0x54, 0x54, 0x5C, 0x18,

  // @443 'f' (7 pixels wide)
  //    ###
  //   ## ##
  //   ##
  // ######
  //   ##
  //   ##
  //   ##
  //
  0x08, 0x08, 0x7E, 0x7F, 0x09, 0x0B, 0x02,

  // @450 'g' (7 pixels wide)
  //
  //
  //  ######
  // ##  ###
  // ##  ###
  //  ######
  //      ##
  //  #####
  0x18, 0xBC, 0xA4, 0xA4, 0xBC, 0xFC, 0x7C,

  // @457 'h' (7 pixels wide)
  // ##
  // ##
  // ######
  // ###  ##
  // ##   ##
  // ##   ##
  // ##   ##
  //
  0x7F, 0x7F, 0x0C, 0x04, 0x04, 0x7C, 0x78,

  // @464 'i' (4 pixels wide)
  //  ##
  //
  // ###
  //  ##
  //  ##
  //  ##
  // ####
  //
  0x44, 0x7D, 0x7D, 0x40,

  // @468 'j' (6 pixels wide)
  //     ##
  //
  //    ###
  //     ##
  //     ##
  //     ##
  // ##  ##
  //  ####
  0x40, 0xC0, 0x80, 0x84, 0xFD, 0x7D,

  // @474 'k' (6 pixels wide)
  // ##
  // ##
  // ##  ##
  // ## ##
  // ####
  // #####
  // ##  ##
  //
  0x7F, 0x7F, 0x30, 0x38, 0x6C, 0x44,

  // @480 'l' (4 pixels wide)
  // ###
  //  ##
  //  ##
  //  ##
  //  ##
  //  ##
  // ####
  //
  0x41, 0x7F, 0x7F, 0x40,

  // @484 'm' (8 pixels wide)
  //
  //
  // #######
  // ## ## ##
  // ## ## ##
  // ## ## ##
  // ## ## ##
  //
  0x7C, 0x7C, 0x04, 0x7C, 0x7C, 0x04, 0x7C, 0x78,

  // @492 'n' (7 pixels wide)
  //
  //
  // ######
  // ###  ##
  // ##   ##
  // ##   ##
  // ##   ##
  //
  0x7C, 0x7C, 0x0C, 0x04, 0x04, 0x7C, 0x78,

  // @499 'o' (7 pixels wide)
  //
  //
  //  #####
  // ##   ##
  // ##   ##
  // ##   ##
  //  #####
  //
  0x38, 0x7C, 0x44, 0x44, 0x44, 0x7C, 0x38,

  // @506 'p' (7 pixels wide)
  //
  //
  // ######
  // ###  ##
  // ###  ##
  // ######
  // ##
  // ##
  0xFC, 0xFC, 0x3C, 0x24, 0x24, 0x3C, 0x18,

  // @513 'q' (7 pixels wide)
  //
  //
  //  ######
  // ##  ###
  // ##  ###
  //  ######
  //      ##
  //      ##
  0x18, 0x3C, 0x24, 0x24, 0x3C, 0xFC, 0xFC,

  // @520 'r' (7 pixels wide)
  //
  //
  // ######
  // ###  ##
  // ##
  // ##
  // ##
  //
  0x7C, 0x7C, 0x0C, 0x04, 0x04, 0x0C, 0x08,

  // @527 's' (7 pixels wide)
  //
  //
  //  ######
  // ##
  //  #####
  //      ##
  // ######
  //
  0x48, 0x5C, 0x54, 0x54, 0x54, 0x74, 0x24,

  // @534 't' (7 pixels wide)
  //   ##
  //   ##
  // ######
  //   ##
  //   ##
  //   ## ##
  //    ###
  //
  0x04, 0x04, 0x3F, 0x7F, 0x44, 0x64, 0x20,

  // @541 'u' (7 pixels wide)
  //
  //
  // ##   ##
  // ##   ##
  // ##   ##
  // ##  ###
  //  ######
  //
  0x3C, 0x7C, 0x40, 0x40, 0x60, 0x7C, 0x7C,

  // @548 'v' (7 pixels wide)
  //
  //
  // ##   ##
  // ##   ##
  // ##   ##
  //  ## ##
  //   ###
  //
  0x1C, 0x3C, 0x60, 0x40, 0x60, 0x3C, 0x1C,

  // @555 'w' (8 pixels wide)
  //
  //
  // ##    ##
  // ## ## ##
  // ## ## ##
  // ## ## ##
  //  ######
  //
  0x3C, 0x7C, 0x40, 0x78, 0x78, 0x40, 0x7C, 0x3C,

  // @563 'x' (7 pixels wide)
  //
  //
  // ##   ##
  //  ## ##
  //   ###
  //  ## ##
  // ##   ##
  //
  0x44, 0x6C, 0x38, 0x10, 0x38, 0x6C, 0x44,

  // @570 'y' (7 pixels wide)
  //
  //
  // ##   ##
  // ##   ##
  // ##  ###
  //  ######
  //      ##
  //  #####
  0x1C, 0xBC, 0xA0, 0xA0, 0xB0, 0xFC, 0x7C,

  // @577 'z' (7 pixels wide)
  //
  //
  // #######
  //     ##
  //   ###
  //  ##
  // #######
  //
  0x44, 0x64, 0x74, 0x54, 0x5C, 0x4C, 0x44,

  // @584 '{' (6 pixels wide)
  //    ###
  //   ##
  //   ##
  // ###
  //   ##
  //   ##
  //    ###
  //
  0x08, 0x08, 0x3E, 0x77, 0x41, 0x41,

  // @590 '|' (2 pixels wide)
  // ##
  // ##
  // ##
  // ##
  // ##
  // ##
  // ##
  // ##
  0xFF, 0xFF,

  // @592 '}' (6 pixels wide)
  // ###
  //   ##
  //   ##
  //    ###
  //   ##
  //   ##
  // ###
  //
  0x41, 0x41, 0x77, 0x3E, 0x08, 0x08,

  // @598 '~' (8 pixels wide)
  //  ###  ##
  // ## ## ##
  // ##  ###
  //
  //
  //
  //
  //
  0x06, 0x07, 0x01, 0x03, 0x06, 0x04, 0x07, 0x03,
  // @606 '~' (5 pixels wide) AEC
  // 1/4 Note
  // ## ## ##
  // ##  ###
  //
  //
  //
  //
  //
  0x60, 0x70, 0x30, 0x30, 0x07f,
  // @611 '~' (7 pixels wide) AEC
  // 1/8th Note
  // ## ## ##
  // ##  ###
  //
  //
  //
  //
  //
  0x60, 0x70, 0x30, 0x30, 0x07f, 0x01, 0x01,
  // @618 '~' (8 pixels wide) AEC
  // 1/3rd Note 
  // ## ## ##
  // ##  ###
  //
  //
  //
  //
  //
  0x15, 0x1f, 0x60, 0x70,0x30, 0x07f, 0x01, 0x01,
  // @626 '~' (7 pixels wide) AEC
  // 16th Note 1/4
  // ## ## ##
  // ##  ###
  //
  //
  //
  //
  //
  0x60, 0x70, 0x30, 0x30, 0x07f, 0x05, 0x05,
// @633 '~' (8 pixels wide) AEC
  // 1/5thh Note
  // ## ## ##
  // ##  ###
  //
  //
  //
  //
  //
  0x17, 0x1d, 0x60, 0x70, 0x30,0x07f, 0x05, 0x05,
  // @641 '~' (8 pixels wide) AEC
  // 1/6 th Note
  // ## ## ##
  // ##  ###
  //
  //
  //
  //
  //
  0x1f, 0x1d, 0x60, 0x70, 0x30,0x07f, 0x05, 0x05,
  //Start Musical Notes
  // @649 'A' (7 pixels wide)
  //   ###
  //  ## ##
  // ##   ##
  // #######
  // ##   ##
  // ##   ##
  // ##   ##
  //
  0x7C, 0x7E, 0x0B, 0x09, 0x0B, 0x7E, 0x7C,
//Start Musical Notes
  // @656 'A#' (8 pixels wide)
  //   ### ##
  //  ## ## #
  // ##   ##
  // #######
  // ##   ##
  // ##   ##
  // ##   ##
  //
  0x7C, 0x7E, 0x0B, 0x09, 0x0B, 0x7E, 0x7C,0x03,
  // @664 'B' (7 pixels wide)
  // ######
  //  ##  ##
  //  ##  ##
  //  #####
  //  ##  ##
  //  ##  ##
  // ######
  //
  0x41, 0x7F, 0x7F, 0x49, 0x49, 0x7F, 0x36,

  // @671 'C' (7 pixels wide)
  //   ####
  //  ##  ##
  // ##
  // ##
  // ##
  //  ##  ##
  //   ####
  //
  0x1C, 0x3E, 0x63, 0x41, 0x41, 0x63, 0x22,

  // @678 'C#' (8 pixels wide)
  //   ####
  //  ##  ##
  // ##
  // ##
  // ##
  //  ##  ##
  //   ####
  //
  0x1C, 0x3E, 0x63, 0x41, 0x41, 0x62, 0x23,0x03,

  // @686 'D' (7 pixels wide)
  // #####
  //  ## ##
  //  ##  ##
  //  ##  ##
  //  ##  ##
  //  ## ##
  // #####
  //
  0x41, 0x7F, 0x7F, 0x41, 0x63, 0x3E, 0x1C,

  // @693 'D#' (8 pixels wide)
  // #####
  //  ## ##
  //  ##  ##
  //  ##  ##
  //  ##  ##
  //  ## ##
  // #####
  //
  0x41, 0x7F, 0x7F, 0x41, 0x63, 0x3E, 0x1d,0x03,

  // @701 'E' (7 pixels wide)
  // #######
  // ##
  // ##
  // #####
  // ##
  // ##
  // #######
  //
  0x7F, 0x7F, 0x49, 0x49, 0x49, 0x41, 0x41,

  // @708 'F' (7 pixels wide)
  // #######
  // ##
  // ##
  // #####
  // ##
  // ##
  // ##
  //
  0x7F, 0x7F, 0x09, 0x09, 0x09, 0x01, 0x01,

  // @715 'F#' (8 pixels wide)
  // #######
  // ##
  // ##
  // #####
  // ##
  // ##
  // ##
  //
  0x7F, 0x7F, 0x09, 0x09, 0x09, 0x00, 0x03,0x03,

  // @723 'G' (7 pixels wide)
  //   ####
  //  ##  ##
  // ##
  // ## ####
  // ##   ##
  //  ##  ##
  //   ####
  //
  0x1C, 0x3E, 0x63, 0x49, 0x49, 0x7B, 0x3A,

  // @730 'G#' (8 pixels wide)
  //   ####
  //  ##  ##
  // ##
  // ## ####
  // ##   ##
  //  ##  ##
  //   ####
  //
  0x1C, 0x3E, 0x63, 0x49, 0x49, 0x78, 0x3B,0x03,

  // @738 Block (8 pixels wide)
  // ########  
   // ######## 
   // ######## 
   // ######## 
   // ######## 
   // ######## 
   // ######## 
  //
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff,
};


// Character descriptors for Terminal_8pt
// { [Char width in bits], [Offset into Terminal_8ptCharBitmaps in bytes] }
const PROGMEM unsigned int Terminal_8ptDescriptors[][2] =
{
  {8, 0}, 		// space 0
  {2, 8}, 		// !
  {5, 10}, 		// "
  {7, 15}, 		// #
  {6, 22}, 		// $
  {7, 28}, 		// %
  {7, 35}, 		// &
  {4, 42}, 		// '
  {4, 46}, 		// (
  {4, 50}, 		// )
  {6, 54}, 		// * 10
  {6, 60}, 		// +
  {3, 66}, 		// ,
  {7, 69}, 		// -
  {3, 76}, 		// .
  {7, 79}, 		// /
  {7, 86}, 		// 0
  {6, 93}, 		// 1
  {7, 99}, 		// 2
  {7, 106}, 		// 3
  {7, 113}, 		// 4 20
  {7, 120}, 		// 5
  {7, 127}, 		// 6
  {7, 134}, 		// 7
  {7, 141}, 		// 8
  {7, 148}, 		// 9
  {2, 155}, 		// :
  {3, 157}, 		// ;
  {7, 160}, 		// <
  {7, 167}, 		// =
  {7, 174}, 		// > 30
  {7, 181}, 		// ?
  {7, 188}, 		// @
  {7, 195}, 		// A
  {7, 202}, 		// B
  {7, 209}, 		// C
  {7, 216}, 		// D
  {7, 223}, 		// E
  {7, 230}, 		// F
  {7, 237}, 		// G
  {7, 244}, 		// H 40
  {4, 251}, 		// I
  {7, 255}, 		// J
  {7, 262}, 		// K
  {7, 269}, 		// L
  {7, 276}, 		// M
  {7, 283}, 		// N
  {7, 290}, 		// O
  {7, 297}, 		// P
  {7, 304}, 		// Q
  {7, 311}, 		// R 50
  {7, 318}, 		// S
  {6, 325}, 		// T
  {7, 331}, 		// U
  {7, 338}, 		// V
  {7, 345}, 		// W
  {7, 352}, 		// X
  {6, 359}, 		// Y
  {7, 365}, 		// Z
  {5, 372}, 		// [
  {7, 377}, 		// backslash 60
  {5, 384}, 		// ] 
  {6, 389}, 		// ^
  {9, 395}, 		// _
  {4, 404}, 		// `
  {7, 408}, 		// a
  {7, 415}, 		// b
  {7, 422}, 		// c
  {7, 429}, 		// d
  {7, 436}, 		// e
  {7, 443}, 		// f 70
  {7, 450}, 		// g 
  {7, 457}, 		// h
  {4, 464}, 		// i
  {6, 468}, 		// j
  {6, 474}, 		// k
  {4, 480}, 		// l
  {8, 484}, 		// m
  {7, 492}, 		// n
  {7, 499}, 		// o
  {7, 506}, 		// p 80
  {7, 513}, 		// q 
  {7, 520}, 		// r
  {7, 527}, 		// s
  {7, 534}, 		// t
  {7, 541}, 		// u
  {7, 548}, 		// v
  {8, 555}, 		// w
  {7, 563}, 		// x
  {7, 570}, 		// y
  {7, 577}, 		// z 90
  {6, 584}, 		// { 
  {2, 590}, 		// |
  {6, 592}, 		// }
  {8, 598}, 		// ~
  {5, 606},     // 16th
  {7,611},      // 8th
  {8,618},      // 1/4th
  {7,626},      // 1/4th
  {8,633},      // 1/5th
  {8,641},      // 1/6th 100
  {7,649},      //A 
{8,656},      //A#
{7,664},      //B
{7,671},      //C
{8,678},      //C#
{7,686},      //D
{8,693},      //D#
{7,701},      //E
{7,708},      //F
{8,715},      //F# 110
{7,723},      //G 
{8,730},      //G#
{8,738},      // block
};

// Font information for Terminal_8pt
const FONT_INFO Terminal_8ptFontInfo =
{
  1, //  Character height
  ' ', //  Start character
  '~', //  End character
  /*	NULL, //  Character block lookup */
  Terminal_8ptDescriptors[0], //  Character descriptor array
  Terminal_8pt, //  Character bitmap array
};






byte _XPos;
byte _YPos;
void Cursor(uint8_t X, uint8_t Y)
{
  _XPos = X;
  _YPos = Y;
}

void DrawMode(byte DrawMode)
{
  _DrawMode = DrawMode;
}


/* Write to a single pixel on the display where:
   X is the x axis coordinate of the pixel
   Y is the Y axis coordinate of the pixel */
void Plot(uint8_t X, uint8_t Y)
{
  byte row = Y / BUFFERROWSIZE;

  if (X < BUFFERCOLSIZE && row < BUFFERROWSIZE)
    if (_DrawMode == NORMAL)
    {
      DisplayBuffer[X][row] |=  (0x01 << (Y % 8));
    } else if (_DrawMode == INVERT)
    {
      DisplayBuffer[X][row] ^=  (0x01 << (Y % 8));
    } else if (_DrawMode == CLEAR)
    {
      DisplayBuffer[X][row] &=  ~(0x01 << (Y % 8));
    }
}

void _Send_Command(byte Data)
{

  digitalWrite(DC_DI, LOW); /* Write to command registers */
  digitalWrite(DC_DI, LOW);
  digitalWrite(CS_DI, LOW);
  SPI.transfer(Data);
  digitalWrite(CS_DI, HIGH);

}

void _DisplayMode(void)
{
  digitalWrite(DC_DI, HIGH);
}

void _CommandMode(void)
{
  digitalWrite(DC_DI, LOW);
}


void ClearBuffer(void)
{
  byte ColIndex;
  byte RowIndex;

  for (RowIndex = 0; RowIndex < 8; RowIndex++)
  {
    for (ColIndex = 0; ColIndex < BUFFERCOLSIZE; ColIndex++)
    {
      DisplayBuffer[ColIndex][RowIndex] = 0x00;
    }
  }
}


/* Flip the horizontal orientation of the screen */
void Flip_H(void)
{
  _H_Ori = ~_H_Ori;

  /* Via SPI interface */

  _CommandMode();
  digitalWrite(CS_DI, LOW);

  if (_H_Ori)
  {
    SPI.transfer(SCANDIRECTIONADD | SCANDIRREVERSE);
  } else
  {
    SPI.transfer(SCANDIRECTIONADD | SCANDIRNORMAL);
  }
  digitalWrite(CS_DI, HIGH);

}



/* Flip the vertical orientation of the screen */
void Flip_V(void)
{
  _V_Ori = ~_V_Ori;

  /* Via SPI interface */

  _CommandMode();
  digitalWrite(CS_DI, LOW);

  if (_V_Ori)
  {
    SPI.transfer(SEGMENTMAPADD | SEGMENTMAPREVERSE);
  } else
  {
    SPI.transfer(SEGMENTMAPADD | SEGMENTMAPNORMAL);
  }
  digitalWrite(CS_DI, HIGH);


}



/* Writes the contents of the display buffer to the display */
void Refresh(void)
{
  byte ColIndex;
  byte RowIndex;
  byte Temp1, Temp2;

  for (RowIndex = 0; RowIndex < _RAM_Pages; RowIndex++)
  {
    /* Only with displays with SH1106 controller that don't support horizontal address mode */

    _Send_Command(PAGEADD | RowIndex);
    _Send_Command(COLLOWADD | _GRAM_Col_Start); /* Lower column address */
    _Send_Command(COLHIGHADD | 0x0); /* Higher column address */


    /* Via SPI interface */

    /* Write to display RAM */
    _DisplayMode();
    for (ColIndex = 0; ColIndex < _Res_Max_X; ColIndex++)
    {
      digitalWrite(CS_DI, LOW);
      SPI.transfer(DisplayBuffer[ColIndex][RowIndex]);
      digitalWrite(CS_DI, HIGH);
    }

  }
}


/* Reset and initialise the module */
void Reset(void)
{

  /* Reset the module */
  if (RST_DI != 0xFF)
  {
    digitalWrite(RST_DI, LOW);
    delay(1);
    digitalWrite(RST_DI, HIGH);
  }

  /* Wait 100mS for DC-DC to stabilise. This can probably be reduced */
  delay(100);

  _Send_Command(SETMUXRATIO); //Set MUX ratio
  _Send_Command(0x3F);

  _Send_Command(CHARGEPUMP); //Enable charge pump
  _Send_Command(ENABLECHARGEPUMP);

  /* Set memory addressing mode to horizontal addressing */
  _Send_Command(MEMORYADDRESSMODE);
  _Send_Command(HORIZONTALADDRESSMODE);


  /* Clear the display buffer */
  ClearBuffer();

  /* Output the display buffer to clear the display RAM */
  Refresh();

  /* Turn display on */
  _Send_Command(DISPLAYONADD | 1);

  /* Wait 150mS for display to turn on */
  delay(150);

  /* Flip the display */
  Flip_H();
  Flip_V();
}




void Bitmap(uint8_t Cols, uint8_t ByteRows, const uint8_t BitmapData[])
{
  byte XIndex;
  byte YIndex;

  byte BufRow;
  byte BufX;

  unsigned int BitmapIndex;

  /* Step through each 8 pixel row */
  for (YIndex = 0; YIndex < ByteRows; YIndex++)
  {
    /* Step through each column */
    for (XIndex = 0; XIndex < Cols; XIndex++)
    {
      BufX = XIndex + _XPos;

      /* If column is beyond display area then don't bother writing to it*/
      if (BufX < BUFFERCOLSIZE)
      {
        BufRow = YIndex + (_YPos / 8);
        BitmapIndex = (YIndex * Cols) + XIndex;

        /* If row is beyond the display area then don't bother writing to it */
        if (BufRow < BUFFERROWSIZE)
          if (_DrawMode == NORMAL)
          {
            DisplayBuffer[BufX][BufRow] |= pgm_read_byte_near(&BitmapData[BitmapIndex]) << (_YPos % 8);
          } else
          {
            DisplayBuffer[BufX][BufRow] ^= pgm_read_byte_near(&BitmapData[BitmapIndex]) << (_YPos % 8);
          }

        /* If column data overlaps to 8 bit rows then write to the second row */
        if (_YPos % 8 && (BufRow + 1) < BUFFERROWSIZE)
          if (_DrawMode == NORMAL)
          {
            DisplayBuffer[BufX][BufRow + 1] |= pgm_read_byte_near(&BitmapData[BitmapIndex]) >> (8 - (_YPos % 8));
          } else
          {
            DisplayBuffer[BufX][BufRow + 1] ^= pgm_read_byte_near(&BitmapData[BitmapIndex]) >> (8 - (_YPos % 8));
          }
      }
    }
  }
}


void _WriteChar(char character)
{
  const byte *FontByteAddress;
  int FontWidth;

  FontByteAddress = _FontType + pgm_read_word_near(_FontDescriptor + (character * 2) + 1);
  FontWidth = pgm_read_word_near(_FontDescriptor + (character * 2));
  Bitmap(FontWidth, _FontHight, FontByteAddress);
  _XPos = _XPos + FontWidth + 2;
}

void Erase(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2)
{
  byte temp = _DrawMode;
  _DrawMode = CLEAR;

  for (int column = X1; column <= X2; column ++)
    for (int row = Y1; row <= Y2; row ++)
      Plot(column, row);

  _DrawMode = temp;
}

void Print(char TextString[])
{
  byte StringLength;
  byte Index;

  /* Finds length of string */
  StringLength = strlen(TextString) - 1;

  for (Index = 0; Index <= StringLength; Index++)
  {
    _WriteChar(TextString[Index] - 32);
  }
}

void Print(long Value)
{
  byte Digits[10];
  int long Temp;
  byte NumDigits = 0;

  /* Is the number negative ? */
  if (Value < 0)
  {
    _WriteChar(13);
    Temp = Value * -1;
  } else
  {
    Temp = Value;
  }

  if (Value == 0) {
    Digits[0] = 0;
    NumDigits = 1;
  }
  /* Store each digit in a byte array so that they
     can be printed in reverse order */
  while (Temp)
  {
    Digits[NumDigits] = Temp % 10;
    Temp /= 10;
    NumDigits++;
  }

  /* Print each digit */
  while (NumDigits)
  {
    NumDigits--;
    _WriteChar(Digits[NumDigits] + 16);
  }
}



/* Draw a line where:
   X1 is the starting X axis coordinate of the line
   Y1 is the starting Y axis coordinate of the line
   X2 is the starting X axis coordinate of the line
   Y2 is the starting Y axis coordinate of the line */
void Line(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2)
{
  double step;
  int x, y;
 
  /* If start coordinates are below and to the right of the end coordinate then flip them */
  if((X2 <= X1 && Y2 <= Y1) || (X2 >= X1 && Y1 >= Y2) || (X2 >= X1 && Y1 >= Y2)) 
  {
    X2^=X1;
    X1^=X2;
    X2^=X1;
    Y2^=Y1;
    Y1^=Y2;
    Y2^=Y1;
  }

  /* If X axis is wider than Y axis then step though X axis */
  if(((X2-X1) >= (Y2-Y1)) || ((X1 - X2) >= (Y2-Y1)))
  {
    step = (double)(Y2-Y1) / (X2-X1);
    if(X2 >= X1 && Y2 >= Y1)
    {
      for(x = X1; x <= X2; x++)
        Plot(x, ((x-X1) * step) + Y1); 
    }else
    {
      for(x = X1; x >= X2; x--)
        Plot(x, Y2 + ((x-X2) * step));
    } 
  }else /* If Y axis is wider than X axis then step though Y axis */
  {
    step = (double)(X2-X1) / (Y2-Y1);
    if(Y2 >= Y1 && X2 >= X1)
    {
      for(y = Y1; y <= Y2; y++)
        Plot(((y-Y1) * step) + X1, y); 
    }else
    {
      for(y = Y2; y >= Y1; y--)
        Plot(X2 + ((y-Y2) * step),y); 
  }
  }
}



/* Draw a rectangle where:
   X1 is the X axis coordinate of the first corner
   Y1 is the Y axis coordinate of the first corner
   X2 is the X axis coordinate of the opposite corner
   Y2 is the Y axis coordinate of the opposite corner 
   FillMode is sets the drawing mode for the rectangle. Options are:
   OUTLINE (Draws an outlined rectangle with no fill
   SOLID (Draws a filled filled rectangle) */
   
void Rect(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, uint8_t FillMode)
{
  byte y;

  /* Draw the top and bottom borders */
  Line(X1, Y1, X2, Y1);
  Line(X1, Y2, X2, Y2);
  
  /* If first corner is below second corner then flip the coordinates */
  if(Y1 > Y2)
  {
    Y2^=Y1;
    Y1^=Y2;
    Y2^=Y1;  
  }
  
  /* If rectangle is wider than two pixels set Y for drawing vertical borders */
  if(Y2-Y1 > 1)
  {
    Y1++;
    Y2--;
  }
 
  /* If box is solid then fill area between top and bottom border */
  if(FillMode == SOLID)
  {
    for(y = Y1; y <= Y2; y++)
      Line(X1, y, X2, y);
  }else  /* if not solid then just draw vertical borders */
  {
    Line(X1, Y1, X1, Y2);
    Line(X2, Y1, X2, Y2); 
  }
}





