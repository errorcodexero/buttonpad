#include <SPI.h>
#include "FastLED.h"

// How many leds are in the strip?
#define NUM_LEDS 64

// Data pin that led data will be written out over      
#define DATA_PIN   5

// pins for reading the buttons
#define RESET_PIN  8
#define LD_PIN     9
#define SS_PIN    10
#define MOSI_PIN  11
#define MISO_PIN  12
#define SCLK_PIN  13

#define DEBOUNCE  30

#define ATTRACT_TIME 20000

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];
byte debounce[NUM_LEDS];
bool button[NUM_LEDS];
byte gameboard[8][8];
byte demoboard[8][8];

unsigned long when;
 
// define the order of the LEDs in the "strip"
const byte ledSequence[] {
   0,  1,  2,  3,  16, 17, 18, 19,
   4,  5,  6,  7,  20, 21, 22, 23,
   8,  9, 10, 11,  24, 25, 26, 27,
  12, 13, 14, 15,  28, 29, 30, 31,
  32, 33, 34, 35,  48, 49, 50, 51,
  36, 37, 38, 39,  52, 53, 54, 55,
  40, 41, 42, 43,  56, 57, 58, 59,
  44, 45, 46, 47,  60, 61, 62, 63,
};

// define the order of the buttons
// note that some of the pins were wired out-of-order
const byte buttonSequence[] {
   15, 14, 13, 12,   7,  6,  5,  4,
   31, 30, 29, 28,  23, 22, 21, 20,
   11, 10,  9,  8,   2,  3,  1,  0,
   27, 26, 25, 24,  19, 18, 17, 16,
   63, 62, 61, 60,  55, 54, 53, 52,
   47, 46, 45, 44,  39, 38, 37, 36,
   43, 42, 41, 40,  35, 34, 33, 32,
   59, 58, 57, 56,  51, 50, 49, 48,
};

const CRGB indexedColor[] {
  CRGB::Black, CRGB::Red,     CRGB::Green,  CRGB::Blue,
  CRGB::Cyan,  CRGB::Magenta, CRGB::Yellow, CRGB::White,
};
  
void showBoard(byte board[8][8])
{
  for (byte i = 0; i < NUM_LEDS; i++) {
    byte row = i / 8;
    byte col = i % 8;
    leds[ledSequence[i]] = indexedColor[board[row][col]];
  }
  FastLED.show();
}

void rect( byte n, byte m, byte color )
{
  if (n < 0 || n >= 8 || m < 0 || m >= 8) return;
  demoboard[n][m] = color;
  demoboard[7-n][m] = color;
  demoboard[n][7-m] = color;
  demoboard[7-n][7-m] = color;
}

void shrink()
{
  demoboard[3][3] = demoboard[2][2];
  demoboard[3][4] = demoboard[2][5];
  demoboard[4][4] = demoboard[5][5];
  demoboard[4][3] = demoboard[5][2];

  demoboard[2][2] = demoboard[1][1];
  demoboard[2][3] = demoboard[1][3];
  demoboard[2][4] = demoboard[1][4];
  demoboard[2][5] = demoboard[1][6];
  demoboard[3][5] = demoboard[3][6];
  demoboard[4][5] = demoboard[4][6];
  demoboard[5][5] = demoboard[6][6];
  demoboard[5][4] = demoboard[6][4];
  demoboard[5][3] = demoboard[6][3];
  demoboard[5][2] = demoboard[6][1];
  demoboard[4][2] = demoboard[4][1];
  demoboard[3][2] = demoboard[3][1];

  demoboard[1][1] = demoboard[0][0];
  demoboard[1][2] = demoboard[0][1];
  demoboard[1][3] = demoboard[0][3];
  demoboard[1][4] = demoboard[0][4];
  demoboard[1][5] = demoboard[0][6];
  demoboard[1][6] = demoboard[0][7];
  demoboard[2][6] = demoboard[1][7];
  demoboard[3][6] = demoboard[3][7];
  demoboard[4][6] = demoboard[4][7];
  demoboard[5][6] = demoboard[6][7];
  demoboard[6][6] = demoboard[7][7];
  demoboard[6][5] = demoboard[7][6];
  demoboard[6][4] = demoboard[7][4];
  demoboard[6][3] = demoboard[7][3];
  demoboard[6][2] = demoboard[7][1];
  demoboard[6][1] = demoboard[7][0];
  demoboard[5][1] = demoboard[6][0];
  demoboard[4][1] = demoboard[4][0];
  demoboard[3][1] = demoboard[3][0];
  demoboard[2][1] = demoboard[1][0];

  demoboard[0][0] = 7;
  demoboard[0][1] = 0;
  demoboard[0][2] = 0;
  demoboard[0][3] = 0;
  demoboard[0][4] = 0;
  demoboard[0][5] = 0;
  demoboard[0][6] = 0;
  demoboard[0][7] = 7;
  demoboard[1][7] = 0;
  demoboard[2][7] = 0;
  demoboard[3][7] = 0;
  demoboard[4][7] = 0;
  demoboard[5][7] = 0;
  demoboard[6][7] = 0;
  demoboard[7][7] = 7;
  demoboard[7][6] = 0;
  demoboard[7][5] = 0;
  demoboard[7][4] = 0;
  demoboard[7][3] = 0;
  demoboard[7][2] = 0;
  demoboard[7][1] = 0;
  demoboard[7][0] = 7;
  demoboard[6][0] = 0;
  demoboard[5][0] = 0;
  demoboard[4][0] = 0;
  demoboard[3][0] = 0;
  demoboard[2][0] = 0;
  demoboard[1][0] = 0;
}

void bigBang()
{
  rect(3, 3, 1);
  showBoard(demoboard);
  delay(40);
  
  rect(2, 2, 1);
  rect(2, 3, 3);
  rect(3, 2, 2);
  showBoard(demoboard);
  delay(40);

  rect(1, 1, 1);
  rect(1, 3, 3);
  rect(3, 1, 2);
  rect(1, 2, 5);
  rect(2, 1, 6);
  showBoard(demoboard);
  delay(40);

  rect(0, 0, 1);
  rect(0, 3, 3);
  rect(3, 0, 2);
  rect(0, 2, 5);
  rect(2, 0, 7);
  rect(0, 1, 4);
  rect(1, 0, 4);
  showBoard(demoboard);
  delay(40);

  rect(3, 3, 0);
  showBoard(demoboard);
  delay(40);

  rect(2, 2, 0);
  rect(2, 3, 0);
  rect(3, 2, 0);
  showBoard(demoboard);
  delay(40);

  rect(1, 1, 0);
  rect(1, 3, 0);
  rect(3, 1, 0);
  rect(1, 2, 0);
  rect(2, 1, 0);
  showBoard(demoboard);
  delay(40);

  rect(0, 0, 0);
  rect(0, 3, 0);
  rect(3, 0, 0);
  rect(0, 2, 0);
  rect(2, 0, 0);
  rect(0, 1, 0);
  rect(1, 0, 0);
}


void attract()
{
  memcpy(demoboard, gameboard, sizeof demoboard);
  
  shrink();
  showBoard(demoboard);
  delay(100);

  shrink();
  showBoard(demoboard);
  delay(100);

  shrink();
  showBoard(demoboard);
  delay(100);

  shrink();
  rect(0, 0, 0);
  showBoard(demoboard);
  delay(100);

  shrink();
  rect(0, 0, 0);
  showBoard(demoboard);
  delay(100);

  shrink();
  rect(0, 0, 0);
  showBoard(demoboard);
  delay(100);

  shrink();
  rect(0, 0, 0);
  showBoard(demoboard);
  delay(300);

  bigBang();
  showBoard(demoboard);
  delay(40);

  bigBang();
  showBoard(demoboard);
  delay(40);

  bigBang();
  showBoard(demoboard);
  delay(40);
}

void setup()
{
  delay(2000);
  
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  
  pinMode(RESET_PIN, OUTPUT);
  pinMode(LD_PIN, OUTPUT);
  pinMode(SS_PIN, OUTPUT);
  pinMode(MOSI_PIN, OUTPUT);
  pinMode(MISO_PIN, INPUT);
  pinMode(SCLK_PIN, OUTPUT);

  digitalWrite(RESET_PIN, LOW);
  digitalWrite(LD_PIN, HIGH);
  digitalWrite(SS_PIN, LOW);
  digitalWrite(MOSI_PIN, HIGH);
  digitalWrite(SCLK_PIN, LOW);
  delayMicroseconds(100);
  digitalWrite(RESET_PIN, HIGH);

  memset(demoboard, 0, sizeof demoboard);
  memset(gameboard, 0, sizeof gameboard);
  memset(debounce, 0, sizeof debounce);
  memset(button, 0, sizeof button);

  attract();
  when = millis();
}

void readButtons()
{
  digitalWrite(LD_PIN, LOW);
  digitalWrite(SCLK_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(SCLK_PIN, LOW);
  digitalWrite(LD_PIN, HIGH);
  for (byte i = 0; i < NUM_LEDS; i++) {
    byte j = buttonSequence[i];
    if (!digitalRead(MISO_PIN)) {
      if (debounce[j] == 0) {
        button[j] = true;
      }
      debounce[j] = DEBOUNCE;
    } else {
      if (debounce[j] > 0) {
        --debounce[j];
      }
    }
    digitalWrite(SCLK_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(SCLK_PIN, LOW);
    delayMicroseconds(10);
  }
}

void loop()
{
  readButtons();
  unsigned long now = millis();
  if ((unsigned long)(now - when) > ATTRACT_TIME) {
    attract();
    when = millis();
  } else {
    for (byte i = 0; i < NUM_LEDS; i++) {
      if (button[i]) {
        byte row = i / 8;
        byte col = i % 8;
        gameboard[row][col] = (gameboard[row][col] + 1) % 8;
        for (byte j = 0; j < row; j++)
          gameboard[j][col] = (gameboard[j][col] + 1) % 8;
        for (byte j = row+1; j < 8; j++)
          gameboard[j][col] = (gameboard[j][col] + 1) % 8;
        for (byte j = 0; j < col; j++)
          gameboard[row][j] = (gameboard[row][j] + 1) % 8;
        for (byte j = col+1; j < 8; j++)
          gameboard[row][j] = (gameboard[row][j] + 1) % 8;
        when = now;
        button[i] = false;
      }
    }
  }
  showBoard(gameboard);
}
