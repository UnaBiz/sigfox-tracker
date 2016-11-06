cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME sigfox-tracker)
project(${PROJECT_NAME})
add_definitions(-DCLION)

# Arduino Options - See https://github.com/queezythegreat/arduino-cmake
# Change this to the Arduino port for flashing the Arduino.
#if(WIN32)
    set(${PROJECT_NAME}_PORT COM4)
#else()
    #set(${PROJECT_NAME}_PORT /dev/tty.usbmodem1421)
#endif()

# Build the library.
set(TinyGPSPlus_SRCS TinyGPSPlus/TinyGPS++.cpp)
set(TinyGPSPlus_HDRS TinyGPSPlus/TinyGPS++.h)
generate_arduino_library(TinyGPSPlus)

set(GroveLCD_SRCS Grove_LCD_RGB_Backlight/rgb_lcd.cpp)
set(GroveLCD_HDRS Grove_LCD_RGB_Backlight/rgb_lcd.h)
generate_arduino_library(GroveLCD)

set(unabiz-arduino_SRCS unabiz-arduino/Radiocrafts.cpp)
set(unabiz-arduino_HDRS unabiz-arduino/Radiocrafts.h unabiz-arduino/SIGFOX.h)
generate_arduino_library(unabiz-arduino)

set(${CMAKE_PROJECT_NAME}_SKETCH sigfox-tracker.ino)
set(${PROJECT_NAME}_ARDLIBS SoftwareSerial)
set(${PROJECT_NAME}_LIBS TinyGPSPlus GroveLCD unabiz-arduino)
generate_arduino_firmware(${CMAKE_PROJECT_NAME})
