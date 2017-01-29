#ifndef ADS1247_h
#define ADS1247_h

class ADS1247
{
  public:
    ADS1247();
    void waitForTrigger();
    double read();
  private:
    const double _VREF;
    const double _PGA;
    const double _LSB;
    byte _DRDYpin;
    byte CSpin;
    void _enableVoltageReference();
    void _reset();
    void _setPGAandSPS();
};

#endif
