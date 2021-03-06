#include "ADS1247.h"
#include <SPI.h>
#include <SD.h>

#define ledPin 13

SPISettings SDSPIsettings = SPISettings(1000000, MSBFIRST, SPI_MODE0);

const int SDchipSelect = 4;

byte ADS1247CSpin = 10;  // Slave select pin for SPI communication to ADS1248.
byte DRDYpin = 3; // interrupt pin - data ready ADS1248
ADS1247 ads1247;

unsigned long time = 0;
unsigned long oldTime = 0;
int dT = 0;

void setup() {
  //Enable Serial to PC (USB)
  Serial.begin(9600);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("start");
  // initialize SPI
  SPI.begin();
  delay(50);
  
  // see if the SD card is present and can be initialized:
  Serial.print("Initializing SD card...");
  if (!SD.begin(SDchipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  } else {
    Serial.println("card initialized.");
  }
  
  // define ads1247 communication class, configures SPI connecetion on Arduino.
  // configures ads1247 for gain, SPS, channels and voltage reference
  ads1247.init(DRDYpin, ADS1247CSpin);
  Serial.println("End init ADS1248");
}

void loop() {
  // wait for interrupt ADS1248
  ads1247.waitForTrigger();
  // Read ADS1248 input channel and convert read value to Volts
  double inputVoltage = ads1247.read();
  // calculate passed time for ANT and debuging
  time = millis();
  int dT = time - oldTime;
  oldTime = time;
  // Write debugging data to PC
  writeResultSerial(inputVoltage, dT);
  
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    String dataString = "";
    dataString += String(inputVoltage);
    dataString += ", ";
    dataString += String(dT);
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  SPI.endTransaction();
}

unsigned long writeResultSerial(double inV, int dT) {
  // Function to write measurement results to the 
  // convert double to string separately for sprintf function
  char inVstr[5];
  dtostrf(inV, 5, 2, inVstr);
  char tbs[40];
  sprintf(tbs, "Val: %s [mV], dT: %2d [ms]", inVstr, dT);
  Serial.println(tbs);
  return oldTime;
}
