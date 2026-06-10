#include <Arduino.h>
#include <SPI.h>
#include <max7456.h>

Max7456       osd(&SPI, 10);

void setup() {
  SPI.begin();

  osd.init();
  osd.setDisplayOffsets(40, 16); // defined from just looking at it

  osd.activateOSD();
  // page 19 of the datasheet
  // top nibble is column of table, bottom nibble is row
  osd.printMax7456Char(0x15, 0, 0); // K
  osd.printMax7456Char(0x20, 1, 0); // V
  osd.printMax7456Char(0x0A, 2, 0); // 0
  osd.printMax7456Char(0x1C, 3, 0); // R
}

void loop() {
}
