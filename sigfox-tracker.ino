#ifdef ARDUINO
  #include <Arduino.h>
#endif  //  ARDUINO
#include "TinyGPSPlus/TinyGPS++.h"

TinyGPSPlus gps;

void setup() {
  String line = "$GPGGA,060556.00,2236.91418,N,11403.24669,E,2,08,1.02,115.1,M,-2.4,M,,0000*43";
}

void loop() {

}