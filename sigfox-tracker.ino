#ifdef ARDUINO
  #include <Arduino.h>
  #ifdef CLION
    //#include <src/SoftwareSerial.h>
    #include "../Program Files (x86)/Arduino/hardware/arduino/avr/libraries/SoftwareSerial/src/SoftwareSerial.h"
  #else  //  CLION
    #include <SoftwareSerial.h>
  #endif  //  CLION
  #include "Grove_LCD_RGB_Backlight/rgb_lcd.h"
#endif  //  ARDUINO

#include "TinyGPSPlus/TinyGPS++.h"

struct Timestamp {  //  Date time from GPS.
  boolean isValid = false;
  uint16_t year = 0;
  uint8_t month = 0;
  uint8_t day = 0;
  uint8_t hour = 0;
  uint8_t minute = 0;
  uint8_t second = 0;
  uint8_t centisecond = 0;
  unsigned long millis = 0;  //  Value of millis() upon update.
} timestamp;

SoftwareSerial receiver(2, 3);  //  Connect GPS receiver to ports RX=2, TX=3.
rgb_lcd lcd;  //  Connect Grove LCD to I2C port 1.
TinyGPSPlus gps;  //  For parsing the GPS output.

//  $GPGSV parameter 3 shows the number of satellites in view being tracked.
TinyGPSCustom satellitesTracked(gps, "GPGSV", 3);

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
    //  Feed the chars from the USB UART to the GPS receiver.
    while (Serial.available() > 0) {
      int ch = Serial.read();
      if (ch >= 0) receiver.print((char) ch);
    }
  }
}

void setup() {
  lcd.begin(16, 2);  //  16 cols, 2 rows.
  lcd.print("Starting");
  Serial.begin(9600); //  Serial.println("Starting");
  receiver.begin(9600);
}

void loop() {
  //  Read all data from the GPS receiver and send to TinyGPS for parsing.
  //  Send all data from USB UART to GPS receiver and vice versa.
  //  This allows the ublox u-center Windows app to interact with the GPS.
  //  Don't show any debug messages to Serial, use LCD instead.

  //  Wait for 1 second to read any updates.
  smartDelay(1000);
  const uint32_t used = gps.satellites.isValid() ? gps.satellites.value() : 0;
  if (!gps.location.isValid()) {
    //  Location not locked yet. Show number of satellites and fixes.
    const uint16_t timecount = (uint16_t) (millis() / 40000);
    const uint32_t fix = gps.sentencesWithFix();
    const char *tracked = satellitesTracked.isValid() ? satellitesTracked.value() : "0";
    String display = String(timecount) + ": sat trk=" + tracked;
    if (fix > 0) display.concat(String(" / fix=") + fix);
    if (used > 0) display.concat(String(" / used=") + used);
    lcd.clear(); lcd.print(display);  //  Serial.print(display);
  }
  else if (gps.location.isUpdated() || gps.altitude.isUpdated()) {
    //  Location updated, get the clock and show the location.
    const double lat = gps.location.lat();
    const double lng = gps.location.lng();
    const double altitude = gps.altitude.meters();
    if (gps.date.isValid() && gps.time.isValid()) {
      //  Read the date time.
      timestamp.year = gps.date.year();
      timestamp.month = gps.date.month();
      timestamp.day = gps.date.day();
      timestamp.hour = gps.time.hour();
      timestamp.minute = gps.time.minute();
      timestamp.second = gps.time.second();
      timestamp.centisecond = gps.time.centisecond();
      timestamp.millis = millis();
      timestamp.isValid = true;
    }

    //  TODO: Send to SIGFOX.
    //  TODO: Log to SD card.

    const String display = String("[") +
        (timestamp.hour < 10 ? String("0") : String("")) + String(timestamp.hour) + "." +
        (timestamp.minute < 10 ? String("0") : String("")) + String(timestamp.minute) + "." +
        (timestamp.second < 10 ? String("0") : String("")) + String(timestamp.second) + "] " +
        String(lat) + " / " + String(lng) + " / " + String(altitude);
    lcd.clear(); lcd.print(display);  //  Serial.println(display);
  }
}
