/***
This example is intended to demonstrate the use of getPixel() versus
getRawPixel() in the GFXcanvas family of classes.

When using the GFXcanvas* classes as the image buffer for a hardware driver,
there is a need to get individual pixel color values at given physical
coordinates. Rather than subclasses or client classes call getBuffer() and
reinterpret the byte layout of the buffer, two methods are added to each of the
GFXcanvas* classes that allow fetching of specific pixel values.

  * getPixel(x, y)   : Gets the pixel color value at the rotated coordinates in
the image.
  * getRawPixel(x,y) : Gets the pixel color value at the unrotated coordinates
in the image. This is useful for getting the pixel value to map to a hardware
pixel location. This method was made protected as only the hardware driver
should be accessing it.

The GFXcanvasSerialDemo class in this example will print to Serial the contents
of the underlying GFXcanvas buffer in both the current rotated layout and the
underlying physical layout.
***/

#include "GFXcanvasSerialDemo.h"
#include <Arduino.h>

void setup() {
  Serial.begin(115200);

  // first create a rectangular GFXcanvasSerialDemo object and draw to it
  GFXcanvasSerialDemo demo(21, 11);

  demo.fillScreen(0x00);
  demo.setRotation(1); // now canvas is 11x21
  demo.fillCircle(5, 10, 5, 0xAA);
  demo.writeLine(0, 0, 10, 0, 0x11);
  demo.writeLine(0, 10, 10, 10, 0x22);
  demo.writeLine(0, 20, 10, 20, 0x33);
  demo.writeLine(0, 0, 0, 20, 0x44);
  demo.writeLine(10, 0, 10, 20, 0x55);

  Serial.println("Demonstrating GFXcanvas rotated and raw pixels.\n");

  // print it out rotated

  Serial.println("The canvas's content in the rotation of '0':\n");
  demo.setRotation(0);
  demo.print(true);
  Serial.println("\n");

  Serial.println("The canvas's content in the rotation of '1' (which is what "
                 "it was drawn in):\n");
  demo.setRotation(1);
  demo.print(true);
  Serial.println("\n");

  Serial.println("The canvas's content in the rotation of '2':\n");
  demo.setRotation(2);
  demo.print(true);
  Serial.println("\n");

  Serial.println("The canvas's content in the rotation of '3':\n");
  demo.setRotation(3);
  demo.print(true);
  Serial.println("\n");

  // print it out unrotated
  Serial.println("The canvas's content in it's raw, physical layout:\n");
  demo.print(false);
  Serial.println("\n");
}

void loop() {}
