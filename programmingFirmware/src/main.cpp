#include <Arduino.h>
#include <SPI.h>
#include <max7456mod.h>

Max7456       osd(&SPI, 9);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // delay(500);
  SPI.setSCK(14);
  SPI.setMOSI(11);
  SPI.setMISO(12);
  SPI.begin();

  

  osd.init();
  osd.setDisplayOffsets(44, 28); // defined from just looking at it

  osd.activateOSD();
  // osd.activateExternalVideo(false);
  // page 19 of the datasheet
  // top nibble is column of table, bottom nibble is row
  osd.printMax7456Char(0x15, 0, 0); // K
  osd.printMax7456Char(0x20, 1, 0); // V
  osd.printMax7456Char(0x0A, 2, 0); // 0
  osd.printMax7456Char(0x1C, 3, 0); // R

  SPI.transfer(0xA0);
  SerialUSB.println(SPI.transfer(0x00), 2);


  delay(5000);

  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(3, HIGH); digitalWrite(4, HIGH);
  digitalWrite(3, LOW); digitalWrite(4, LOW);
  delay(300);
  digitalWrite(3, HIGH); digitalWrite(4, HIGH);
}

void loop() {}