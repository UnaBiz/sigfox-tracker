//  Test the transceiver functions under Windows or Mac without Arduino.
#ifndef ARDUINO
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "util.cpp"

#include "../TinyGPSPlus/TinyGPS++.cpp"
#include "../sigfox-tracker.ino"

String test_line = "$GPGGA,060556.00,2236.91418,N,11403.24669,E,2,08,1.02,115.1,M,-2.4,M,,0000*43\n";

int main() {
  puts("Starting test...");
  receiver.setBuffer(test_line.c_str());

  setup();
  for (;;) {
    loop();
    delay(5000);
    break;
  }
  return 0;
}
#endif  //  ARDUINO
