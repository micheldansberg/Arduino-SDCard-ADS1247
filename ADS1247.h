#ifndef ADS1247_h
#define ADS1247_h

#include <SPI.h>
#include <Arduino.h>
#include <stdio.h>

class ADS1247
{
  public:
    ADS1247();
    void init(byte _DRDYpinInput, byte _CSpinInput);
    void waitForTrigger();
    double read();
  private:
    int _SPIclock1MHZ;
    byte _DRDYpin;
    byte _CSpin;
    double _VREF;
    double _PGA;
    double _LSB;
    void _enableVoltageReference();
    void _reset();
    void _setPGAandSPS();
};

#endif
