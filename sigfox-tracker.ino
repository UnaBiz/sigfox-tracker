#include <SIGFOX.h>
#include <TinyGPS++.h>

#ifdef ARDUINO
  #ifdef CLION
    //#include <src/SoftwareSerial.h>
    //#include "../Program Files (x86)/Arduino/hardware/arduino/avr/libraries/SoftwareSerial/src/SoftwareSerial.h"
  #endif  //  CLION
  #include "Grove_LCD_RGB_Backlight/rgb_lcd.h"
#endif  //  ARDUINO

//  IMPORTANT: Check these settings with UnaBiz to use the SIGFOX library correctly.
static const String device = "g88pi";  //  Set this to your device name if you're using UnaBiz Emulator.
static const bool useEmulator = false;  //  Set to true if using UnaBiz Emulator.
static const bool echo = true;  //  Set to true if the SIGFOX library should display the executed commands.
static const Country country = COUNTRY_SG;  //  Set this to your country to configure the SIGFOX transmission frequencies.
static Radiocrafts transceiver(country, useEmulator, device, echo);  //  Uncomment this for UnaBiz UnaShield Dev Kit with Radiocrafts module.
#define DELAY_PER_MESSAGE 2  //  How many seconds to wait before sending the next message.

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

#ifdef BEAN_BEAN_BEAN_H
  BeanSoftwareSerial receiver(2, 3);  //  Connect GPS receiver to ports RX=2, TX=3.
  #define setLed(r, g, b) Bean.setLed(r, g, b)

#else  //  BEAN_BEAN_BEAN_H
  SoftwareSerial receiver(2, 3);  //  Connect GPS receiver to ports RX=2, TX=3.
  #define setLed(r, g, b) {}
#endif  //  BEAN_BEAN_BEAN_H

#ifdef NOTUSED
  rgb_lcd lcd;  //  Connect Grove LCD to I2C port 1.
#endif // NOTUSED
TinyGPSPlus gps;  //  For parsing the GPS output.

//  $GPGSV parameter 3 shows the number of satellites in view being tracked.
TinyGPSCustom satellitesTracked(gps, "GPGSV", 3);

static void smartDelay(unsigned long ms) {
  //  This custom version of delay() ensures that the gps object is being "fed".
  receiver.listen();  //  Must listen because SIGFOX may have changed listener.
  unsigned long start = millis();
  for (;;) {
    //  Quit if we have waited long enough.
    if (millis() - start >= ms) break;
    //  Feed the chars from GPS receiver to TinyGPS.
    while (receiver.available() > 0) {
      int ch = receiver.read();
      if (ch >= 0) {
#ifdef NOTUSED
        Serial.print((char) ch);
#endif // NOTUSED
        gps.encode((char) ch);
      }
      if (millis() - start >= ms) break;
    }
#ifdef NOTUSED
    //  Feed the chars from the USB UART to the GPS receiver.
    while (Serial.available() > 0) {
      int ch = Serial.read();
      if (ch >= 0) receiver.print((char) ch);
    }
#endif // NOTUSED
  }
}

void setup() {
#ifdef NOTUSED
  lcd.begin(16, 2);  lcd.print("Starting");  //  16 cols, 2 rows.
#endif  // NOTUSED
  //  Initialize console so we can see debug messages (9600 bits per second).
  Serial.begin(9600);  Serial.println(F("Running setup..."));
  receiver.begin(9600);
  setLed(0, 0, 255);  //  Blue

  //  Check whether the SIGFOX module is functioning.
  while (!transceiver.begin())
    Serial.println(F("Unable to init SIGFOX module, may be missing"));
}

int page = 0;

void loop() {
  //  Read all data from the GPS receiver and send to TinyGPS for parsing.
  //  Send all data from USB UART to GPS receiver and vice versa.
  //  This allows the ublox u-center Windows app to interact with the GPS.
  //  Don't show any debug messages to Serial, use LCD instead.
  
  //  Wait for a few seconds to read any updates.
  smartDelay(DELAY_PER_MESSAGE * 1000);
  setLed(0, 0, 0);

  //  Check whether we have gotten the GPS location.
  const uint8_t used = (uint8_t) gps.satellites.isValid() ? gps.satellites.value() : 0;
  if (!gps.location.isValid()) {
    //  Location not locked yet. Show number of satellites and fixes.
    const uint16_t timecount = (uint16_t) (millis() / 40000);
    const uint32_t fix = gps.sentencesWithFix();
    const char *tracked = satellitesTracked.isValid() ? satellitesTracked.value() : "0";
    String display = String(timecount) + ": sat trk=" + tracked;
    if (fix > 0) display.concat(String(" / fix=") + fix);
    if (used > 0) display.concat(String(" / used=") + used);
#ifdef NOTUSED
    lcd.clear(); lcd.print(display);
#endif  //  NOTUSED
    Serial.print(display);
    setLed(0xff, 0xa5, 0);  //  Orange
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

    //  Convert to hex and combine them.
    String lat2 = transceiver.toHex((float) lat);
    String lng2 = transceiver.toHex((float) lng);
    String altitude2 = transceiver.toHex((int) (altitude * 10));
    String used2 = transceiver.toHex((int) used);
    Serial.print("lat="); Serial.print(lat); Serial.print(" / "); Serial.println(lat2);
    Serial.print("lng="); Serial.print(lng); Serial.print(" / "); Serial.println(lng2);
    Serial.print("altitude="); Serial.print(altitude); Serial.print(" / "); Serial.println(altitude2);
    Serial.print("used="); Serial.print(used); Serial.print(" / "); Serial.println(used2);
    String msg = lat2 + lng2 + altitude2 + used2;

    //  Send to SIGFOX.
    if (transceiver.sendMessage(msg)) {
      Serial.println(F("Message sent!"));
      setLed(0, 255, 0);  //  Green
    } else {
      Serial.println(F("Message not sent!"));
      setLed(255, 0, 0);  //  Red
    }
    //  TODO: Save the GPS state so that GPS tracking is faster next time.
    //  TODO: Log to SD card.
#if NOTUSED
    const String display = String("[") +
        (timestamp.hour < 10 ? String("0") : String("")) + String(timestamp.hour) + "." +
        (timestamp.minute < 10 ? String("0") : String("")) + String(timestamp.minute) + "." +
        (timestamp.second < 10 ? String("0") : String("")) + String(timestamp.second) + "] " +
        String(lat) + " / " + String(lng) + " / " + String(altitude) + " / " + String(used);
    const String displayPage = display.substring(page * 16, (page + 1) * 16);
    if (displayPage.length() == 0) page = 0;
    else {
      page++;
      lcd.clear(); lcd.print(displayPage);  Serial.println(displayPage);
    }
#endif
  }
}
