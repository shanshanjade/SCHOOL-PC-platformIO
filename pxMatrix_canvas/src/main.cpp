#include <Arduino.h>
#include <PxMatrix.h>
#define P_A 2
#define P_B 3
#define P_C 4
#define P_D 5
#define P_E 6
#define P_LAT 7
#define P_OE 8

//PxMATRIX display(32,16,P_LAT, P_OE,P_A,P_B,P_C);
PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
GFXcanvas1 mycanvas(32, 32);

void setup() {
  // put your setup code here, to run once:
  mycanvas.getBuffer();
  display.drawBitmap(32, 16, mycanvas.getBuffer(), 32, 16, 0xFE00);
}

void loop() {
  // put your main code here, to run repeatedly:
}