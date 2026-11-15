# Adafruit GFX Library
# https://github.com/adafruit/Adafruit-GFX-Library
# BSD License

cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "src/Adafruit_GFX.cpp" "src/Adafruit_GrayOLED.cpp" "src/Adafruit_SPITFT.cpp" "src/glcdfont.c"
                       INCLUDE_DIRS "src"
                       REQUIRES arduino-esp32 Adafruit_BusIO)

project(Adafruit-GFX-Library)
