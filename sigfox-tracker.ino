#include "unabiz-arduino/SIGFOX.h"
#include "TinyGPSPlus/TinyGPS++.h"

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
  //  This custom version of delay() ensures that the gps object is being "fed".
  receiver.listen();  //  Must listen because SIGFOX may have changed listener.
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

void setupSIGFOX() {
    String result = "";
  //  Enter command mode.  TODO: Confirm response = '>'
  Serial.println(F("\nEntering command mode (expecting '>')..."));
  transceiver.enterCommandMode();

  //  Disable emulation mode.
  Serial.println(F("\nDisabling emulation mode..."));
  transceiver.disableEmulator(result);

  //  Check whether emulator is used for transmission.
  Serial.println(F("\nChecking emulation mode (expecting 0)...")); int emulator = 0;
  transceiver.getEmulator(emulator);

  //  Get network mode for transmission.  Should return network mode = 0 for uplink only, no downlink.
  Serial.println(F("\nGetting network mode (expecting 0)..."));
  transceiver.getParameter(0x3b, result);

  //  Get baud rate.  Should return baud rate = 5 for 19200 bps.
  Serial.println(F("\nGetting baud rate (expecting 5)..."));
  transceiver.getParameter(0x30, result);

  //  Set the frequency of SIGFOX module to SG/TW.
  Serial.println(F("\nSetting frequency..."));  result = "";
  transceiver.setFrequencySG(result);
  Serial.print(F("Set frequency result = "));  Serial.println(result);

  //  Get and display the frequency used by the SIGFOX module.  Should return 3 for RCZ4 (SG/TW).
  Serial.println(F("\nGetting frequency (expecting 3)..."));  String frequency = "";
  transceiver.getFrequency(frequency);
  Serial.print(F("Frequency (expecting 3) = "));  Serial.println(frequency);

  //  Read module temperature.
  Serial.println(F("\nGetting temperature..."));  int temperature = 0;
  if (transceiver.getTemperature(temperature))
  {
    Serial.print(F("Temperature = "));  Serial.print(temperature);  Serial.println(F(" C"));
  }
  else
  {
    Serial.println(F("Temperature KO"));
  }

  //  Read module supply voltage.
  Serial.println(F("\nGetting voltage..."));  float voltage = 0.0;
  if (transceiver.getVoltage(voltage))
  {
    Serial.print(F("Supply voltage = "));  Serial.print(voltage);  Serial.println(F(" V"));
  }
  else
  {
    Serial.println(F("Supply voltage KO"));
  }

  //  Read SIGFOX ID and PAC from module.
  Serial.println(F("\nGetting SIGFOX ID..."));  String id = "", pac = "";
  if (transceiver.getID(id, pac))
  {
    Serial.print(F("SIGFOX ID = "));  Serial.println(id);
    Serial.print(F("PAC = "));  Serial.println(pac);
  }
  else
  {
    Serial.println(F("ID KO"));
  }

  //  Read power.
  Serial.println(F("\nGetting power..."));  int power = 0;
  if (transceiver.getPower(power))
  {
    Serial.print(F("Power level = "));  Serial.print(power);  Serial.println(F(" dB"));
  }
  else
  {
    Serial.println(F("Power level KO"));
  }

  //  Exit command mode and prepare to send message.
  transceiver.exitCommandMode();
}

void setup() {
  lcd.begin(16, 2);  //  16 cols, 2 rows.
  lcd.print("Starting");
  Serial.begin(9600); //  Serial.println("Starting");
  receiver.begin(9600);

  transceiver.echoOn();  //  Comment this line to hide the echoing of commands.
  //  Check whether the SIGFOX module is functioning.
  if (!transceiver.begin())
  {
    Serial.println(F("Error: SIGFOX Module KO!"));
    //for(;;) {}  //  Loop forever because we can't continue.
  }
  setupSIGFOX();
}

int page = 0;

void loop() {
  //  Read all data from the GPS receiver and send to TinyGPS for parsing.
  //  Send all data from USB UART to GPS receiver and vice versa.
  //  This allows the ublox u-center Windows app to interact with the GPS.
  //  Don't show any debug messages to Serial, use LCD instead.
  
  //  Wait for 6 seconds to read any updates.
  smartDelay(6000);

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
    lcd.clear(); lcd.print(display);  Serial.print(display);
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
    Serial.println(String("lat=") + lat + " / " + lat2);
    Serial.println(String("lng=") + lng + " / " + lng2);
    Serial.println(String("altitude=") + altitude + " / " + altitude2);
    Serial.println(String("used=") + used + " / " + used2);
    String msg = lat2 + lng2 + altitude2 + used2;

    //  Send to SIGFOX.
    if (transceiver.sendMessage(msg))
    {
      Serial.println(F("Message sent!"));
    }
    else
    {
      Serial.println(F("Message not sent!"));
    }

    //  TODO: Save the GPS state so that GPS tracking is faster next time.
    //  TODO: Log to SD card.

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
  }
}
