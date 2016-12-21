## sigfox-tracker
# GPS tracker with logging to SIGFOX based on UnaKit development kit by UnaBiz

This Arduino project periodically transmits the latitude, longitude, altitude and timestamp
info received from the GPS module to the SIGFOX cloud. Hardware requirements:

0. [UnaShield Arduino development kit](https://github.com/UnaBiz/unabiz-arduino/wiki/UnaShield) for SIGFOX by UnaBiz
0. DFRobot GPS module
0. Grove LCD display

## GPS

This project has been tested with the DFRobot GPS, which is based on the u-blox UBX-M8030-KT GPS module:

https://www.dfrobot.com/index.php?route=product/product&path=57&product_id=1302

https://www.dfrobot.com/wiki/index.php/GPS_Module_With_Enclosure_(SKU:TEL0094)

https://github.com/CainZ/V.KEL-GPS/raw/master/VK2828U7G5LF%20Data%20Sheet%2020150902.pdf

https://www.u-blox.com/en/product-resources?f[0]=property_file_product_filter%3A2696

https://www.u-blox.com/sites/default/files/products/documents/u-blox8-M8_ReceiverDescrProtSpec_%28UBX-13003221%29_Public.pdf

The u-center Windows application may be used to connect to the GPS module on
the Arduino board to run diagnostics:

https://github.com/CainZ/V.KEL-GPS/raw/master/u-centersetup_v8.12.zip

## LCD Display

This project uses the Grove LCD display for displaying the status of the GPS reception:

http://wiki.seeed.cc/Grove-LCD_RGB_Backlight/

## UnaKit

This project calls the UnaShield library for sending GPS data to the SIGFOX cloud:

https://github.com/UnaBiz/unabiz-arduino

## Connecting the hardware

Connect the hardware components as follows:

0. UnaShield / Arduino D2 (RX) <--> GPS TX
0. UnaShield / Arduino D3 (TX) <--> GPS RX
0. UnaShield / Arduino SCL (I2C) <--> Grove LCD SCL
0. UnaShield / Arduino SDA (I2C) <--> Grove LCD SDA
0. UnaShield / Arduino 5V <--> GPS VCC
0. UnaShield / Arduino 5V <--> Grove LCD VCC
0. UnaShield / Arduino GND <--> GPS GND
0. UnaShield / Arduino GND <--> Grove LCD GND

Note: UnaShield uses Arduino ports D4 (TX), D5 (RX)

## Installing the software

```
git clone https://github.com/UnaBiz/sigfox-tracker.git
cd sigfox-tracker
git clone https://github.com/UnaBiz/unabiz-arduino.git
```

Launch the Arduino IDE and open `sigfox-tracker/sigfox-tracker.ino`

## Message Format

The message sent to SIGFOX consists of 12 bytes:

0. Latitude, in degrees (encoded double, 4 bytes)
0. Longitude, in degrees (encoded double, 4 bytes)
0. Altitude * 10, in metres (encoded integer, 2 bytes)
0. Number of satellites used (encoded integer, 2 bytes)

## Test Program

The `test` folder contains a test program that compiles the project source files
into a Windows / Mac executable that is easier to debug.  Open the `test` folder
in CLion to run it on your Windows / Mac.

## LightBlue Bean+ now supported

This sketch works with the **[LightBlue Bean+ by Punch Through](https://punchthrough.com/bean)** as described below.
We call this the *UnaTumbler.*

https://github.com/UnaBiz/unabiz-arduino/wiki/UnaShield

![UnaTumbler SIGFOX Tracker](https://github.com/UnaBiz/media/blob/master/sigfox-tracker/unatumbler-bus.jpg)

## SIGFOX Tracker with Arduino Uno

Here is the original SIGFOX Tracker with UnaShield and Arduino Uno.

![SIGFOX Tracker with Arduino Uno](https://github.com/UnaBiz/media/blob/master/sigfox-tracker/sigfox-tracker.jpg)
