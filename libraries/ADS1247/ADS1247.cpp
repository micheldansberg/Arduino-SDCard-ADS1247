#include "Arduino.h"
#include "ADS1247.h"
#include "SPI.h"
#include "stdio.h"

ADS1247::ADS1247(byte DRDYpin, byte CSpin) {
	_VREF = 1.024;  // reference voltage ADS1247 2.048V/2 is the designed value.
	_PGA = 128.0;
	_LSB = (_VREF / _PGA) / (pow(2, 23) - 1);

	// Configure Arduino pins for ADS 1247 communication
	_CSpin = CSpin;
	_DRDYpin = DRDYpin;
	pinMode(_DRDYpin, INPUT);
	pinMode(_CSpin, OUTPUT);

	//24-bit AD conververt Setup for ADS1247
	//Configure SPI Bus on Arduino
	SPI.setClockDivider(SPI_CLOCK_DIV8); // pro micro is 8Mhz
	SPI.setDataMode(SPI_MODE1);          // SPI timing protocol: I think it is read signal when clock rises .
	delay(500);
	SPI.begin();
	// Reset. Not sure if this is required.
	_reset();
	_setPGAandSPS();
	_enableVoltageReference();
}

void ADS1247::waitForTrigger() {
  byte DRDY = 0;          // trigger variable - HIGH or LOW
  DRDY = digitalRead(_DRDYpin);
  while (DRDY == HIGH) {
    delay(1);
    DRDY = digitalRead(_DRDYpin);
  };
};

double ADS1247::read() {
  long A2D = 0x0;
  digitalWrite(_CSpin, LOW);
  A2D |= SPI.transfer(0xFF);
  A2D <<= 8;
  A2D |= SPI.transfer(0xFF);
  A2D <<= 8;
  A2D |= SPI.transfer(0xFF);
  SPI.transfer(0xFF);
  digitalWrite(_CSpin, HIGH);
  // Convert signs if needed
  if (A2D & 0x800000) {
    A2D |= ~0xFFFFFF;
  }
  double inV = 1000.0 * _LSB * A2D;
  return inV;
};

void ADS1247::_enableVoltageReference() {
  // Enable voltage reference
  digitalWrite(_CSpin, LOW);
  SPI.transfer(0x42); //Set MUX1 Register (02h)
  SPI.transfer(0x00);
  SPI.transfer(0x20); //Internal reference is always on
  SPI.transfer(0xFF);
  digitalWrite(_CSpin, HIGH);
};

void ADS1247::_reset() {
  digitalWrite(_CSpin, LOW);
  SPI.transfer(0x06); //Reset
  SPI.transfer(0xFF);
  digitalWrite(_CSpin, HIGH);
  delay(1000);
};

void ADS1247::_setPGAandSPS() {
  //Set gain (PGA) and measurments per second (SPS)
  digitalWrite(_CSpin, LOW);
  SPI.transfer(0x43); //Set SYS0 Register (03h)
  SPI.transfer(0x00);
  SPI.transfer(0x70); //PGA = 128 & 80SPS
  SPI.transfer(0xFF);
  digitalWrite(_CSpin, HIGH);
};