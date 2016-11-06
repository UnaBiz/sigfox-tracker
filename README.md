## sigfox-tracker
# GPS tracker with logging to SIGFOX based on UnaKit development kit by UnaBiz

This Arduino project periodically transmits the latitude, longitude, altitude and timestamp
info received from the GPS module to the SIGFOX cloud. Hardware requirements:

0. UnaKit Arduino development kit for SIGFOX by UnaBiz
0. DFRobot GPS module
0. Grove LCD display

## GPS

This project has been tested with the DFRobot GPS:

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

This project calls the UnaKit library for sending GPS data to the SIGFOX cloud:

https://github.com/UnaBiz/unabiz-arduino

## Connection

0. Connect GPS receiver to Arduino ports RX=D2, TX=D3
0. Connect UnaKit to Arduino ports RX=D5, TX=D4
0. Connect Grove LCD to Arduino I2C port 1

## Message Format

The message sent to SIGFOX consists of 13 bytes:

0. Latitude (encoded double, 4 bytes)
0. Longitude (encoded double, 4 bytes)
0. Altitude (encoded double, 4 bytes)
0. Number of satellites used (integer, 1 byte)

## test

The `test` folder contains a test program that compiles the project source files
into a Windows / Mac executable that is easier to debug.  Open the `test` folder
in CLion to run it on your Windows / Mac.

