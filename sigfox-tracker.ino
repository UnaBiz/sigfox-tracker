#ifdef ARDUINO
  #include <Arduino.h>
  #ifdef CLION
    #include <src/SoftwareSerial.h>
    //#include "../Program Files (x86)/Arduino/hardware/arduino/avr/libraries/SoftwareSerial/src/SoftwareSerial.h"
  #else  //  CLION
    #include <SoftwareSerial.h>
  #endif  //  CLION
  #include "Grove_LCD_RGB_Backlight/rgb_lcd.h"
#endif  //  ARDUINO

#include "TinyGPSPlus/TinyGPS++.h"

SoftwareSerial receiver(2, 3);  //  Connect GPS receiver to ports RX=2, TX=3.
rgb_lcd lcd;  //  Connect Grove LCD to I2C port 1.
TinyGPSPlus gps;

void setup() {
  lcd.begin(16, 2);  //  16 cols, 2 rows.
  lcd.print("Starting");
  Serial.begin(9600); //  Serial.println("Starting");
  receiver.begin(9600);
}

static void smartDelay(unsigned long ms)
{
  // This custom version of delay() ensures that the gps object
  // is being "fed".
  unsigned long start = millis();
  for (;;)
  {
    //  Quit if we have waited long enough.
    if (millis() - start >= ms) break;
    //  Feed the chars from GPS receiver to TinyGPS.
    while (receiver.available() > 0) {
      int ch = receiver.read();
      if (ch >= 0) {
        Serial.print((char) ch);
        gps.encode((char) ch);
      }
    }
    while (Serial.available() > 0) {
      int ch = Serial.read();
      if (ch >= 0) receiver.print((char) ch);
    }
  }
}

void loop() {
  //  Read all data from the GPS receiver and send to TinyGPS for parsing.
  //  TODO: Get GPS clock.
  //  Serial.print('.');
  smartDelay(1000);
  if (!gps.location.isValid()) {
    //  Location not locked yet. Show number of satellites and fixes.
    const uint32_t sat = gps.satellites.value();
    const uint32_t fix = gps.sentencesWithFix();
    const uint16_t tm = (uint16_t) (millis() / 40000);
    const String display = String(tm) + ": fix=" + fix + ", sat=" + sat;
    lcd.clear(); lcd.print(display);  //  Serial.print(display);
  }
  else if (gps.location.isUpdated() || gps.altitude.isUpdated()) {
    //  Location updated, show the location.
    const double lat = gps.location.lat();
    const double lng = gps.location.lng();
    const double altitude = gps.altitude.meters();

    //  TODO: Send to SIGFOX.
    //  TODO: Log to SD card.

    const String display = String(lat) + "," + String(lng) + "," + String(altitude);
    lcd.clear(); lcd.print(display);  //  Serial.println(display);
  }
}
