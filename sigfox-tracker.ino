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
  Serial.begin(9600);
  Serial.println("Starting");
  lcd.begin(16, 2);  //  16 cols, 2 rows.
  lcd.print("Starting");
  receiver.begin(9600);
}

void loop() {
  //  Read all data from the GPS receiver and send to TinyGPS for parsing.
  Serial.print('.');
  while (receiver.available() > 0) {
    char ch = (char) receiver.read();
    Serial.print(ch);
    gps.encode(ch);
  }
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
