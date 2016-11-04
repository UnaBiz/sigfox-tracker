#ifdef ARDUINO
  #include <Arduino.h>
  #ifdef CLION
    #include <src/SoftwareSerial.h>
  #else  //  CLION
    #include <SoftwareSerial.h>
  #endif  //  CLION
  #include "Grove_LCD_RGB_Backlight/rgb_lcd.h"
#endif  //  ARDUINO

#include "TinyGPSPlus/TinyGPS++.h"
TinyGPSPlus gps;
rgb_lcd lcd;

#ifdef ARDUINO
  //  Assume GPS receiver is connected to ports RX=2, TX=3.
  SoftwareSerial receiver(2, 3); // RX, TX
#else  //  ARDUINO
  String test_line = "$GPGGA,060556.00,2236.91418,N,11403.24669,E,2,08,1.02,115.1,M,-2.4,M,,0000*43\n";
  Print receiver(test_line.c_str());
#endif  //  ARDUINO

void setup() {
  lcd.begin(16, 2);
}

void loop() {
  while (receiver.available() > 0)
    gps.encode((char) receiver.read());
#ifdef ARDUINO
#else  //  ARDUINO
#endif //  ARDUINO
  if (gps.altitude.isUpdated()) {
    const double lat = gps.location.lat();
    const double lng = gps.location.lng();
    const double altitude = gps.altitude.meters();
    Serial.print("lat="); Serial.print(lat, 6);
    Serial.print(", lng="); Serial.print(lng, 6);
    Serial.print(", alt="); Serial.println(altitude);

    const String display = String(lat, 2) + "," +
        String(lng, 2) + "," + String(altitude, 0);
    lcd.print(display);
  }
  delay(1000);
}
