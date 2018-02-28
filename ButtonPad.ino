#include <SPI.h>
#include "FastLED.h"

// How many leds are in the strip?
#define NUM_LEDS 64

// Data pin that led data will be written out over      
#define DATA_PIN 3

// pins for reading the buttons
#define LD_PIN    9
#define SS_PIN    10
#define MOSI_PIN  11
#define MISO_PIN  12
#define SCLK_PIN  13

#define DEBOUNCE  50

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];
byte button[NUM_LEDS];
byte gameboard[8][8];
 
// define the order of the LEDs in the "strip"
const int ledSequence[] {
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
const int buttonSequence[] {
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
  
void showBoard()
{
  for (int i = 0; i < NUM_LEDS; i++) {
    int row = i / 8;
    int col = i % 8;
    leds[ledSequence[i]] = indexedColor[gameboard[row][col]];
  }
  FastLED.show();
}

void rect( int n, int m, int color )
{
  if (n < 0 || n >= 8 || m < 0 || m >= 8) return;
  gameboard[n][m] = color;
  gameboard[7-n][m] = color;
  gameboard[n][7-m] = color;
  gameboard[7-n][7-m] = color;
}

void zoomIn()
{
  for (int n = 0; n < 4; n++) {
    rect(n, n, 7);
    showBoard();
    delay(100);
    rect(n, n, 0);
  }
}

void bigBang()
{
  rect(3, 3, 1);
  showBoard();
  delay(40);
  
  rect(2, 2, 1);
  rect(2, 3, 3);
  rect(3, 2, 2);
  showBoard();
  delay(40);

  rect(1, 1, 1);
  rect(1, 3, 3);
  rect(3, 1, 2);
  rect(1, 2, 5);
  rect(2, 1, 6);
  showBoard();
  delay(40);

  rect(0, 0, 1);
  rect(0, 3, 3);
  rect(3, 0, 2);
  rect(0, 2, 5);
  rect(2, 0, 7);
  rect(0, 1, 4);
  rect(1, 0, 4);
  showBoard();
  delay(40);

  rect(3, 3, 0);
  showBoard();
  delay(40);

  rect(2, 2, 0);
  rect(2, 3, 0);
  rect(3, 2, 0);
  showBoard();
  delay(40);

  rect(1, 1, 0);
  rect(1, 3, 0);
  rect(3, 1, 0);
  rect(1, 2, 0);
  rect(2, 1, 0);
  showBoard();
  delay(40);

  rect(0, 0, 0);
  rect(0, 3, 0);
  rect(3, 0, 0);
  rect(0, 2, 0);
  rect(2, 0, 0);
  rect(0, 1, 0);
  rect(1, 0, 0);
  showBoard();
  delay(40);
}

void setup()
{
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  
  pinMode(LD_PIN, OUTPUT);
  pinMode(SS_PIN, OUTPUT);
  pinMode(MOSI_PIN, OUTPUT);
  pinMode(MISO_PIN, INPUT);
  pinMode(SCLK_PIN, OUTPUT);

  digitalWrite(LD_PIN, HIGH);
  digitalWrite(SS_PIN, LOW);
  digitalWrite(MOSI_PIN, HIGH);
  digitalWrite(SCLK_PIN, LOW);

  zoomIn();
  delay(200);
  bigBang();
  bigBang();
  bigBang();
}

void readButtons()
{
  digitalWrite(LD_PIN, LOW);
  digitalWrite(SCLK_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(SCLK_PIN, LOW);
  digitalWrite(LD_PIN, HIGH);
  for (int i = 0; i < NUM_LEDS; i++) {
    if (digitalRead(MISO_PIN)) {
      button[buttonSequence[i]] = 0;
    } else {
      if (button[buttonSequence[i]] < 255)
        ++button[buttonSequence[i]];
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
  for (int i = 0; i < NUM_LEDS; i++) {
    if (button[i] == DEBOUNCE) {
      int row = i / 8;
      int col = i % 8;
      gameboard[row][col] = (gameboard[row][col] + 1) % 8;
      for (int j = 0; j < row; j++)
        gameboard[j][col] = (gameboard[j][col] + 1) % 8;
      for (int j = row+1; j < 8; j++)
        gameboard[j][col] = (gameboard[j][col] + 1) % 8;
      for (int j = 0; j < col; j++)
        gameboard[row][j] = (gameboard[row][j] + 1) % 8;
      for (int j = col+1; j < 8; j++)
        gameboard[row][j] = (gameboard[row][j] + 1) % 8;
    }
  }
  showBoard();
}
