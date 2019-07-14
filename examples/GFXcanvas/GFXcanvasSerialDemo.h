#ifndef __GFXcanvasSerialDemo__
#define __GFXcanvasSerialDemo__
#include <Adafruit_GFX.h>

/**********************************************************************/
/*!
  @brief	Demonstrates using the GFXconvas classes as the backing store
  for a device driver.
*/
/**********************************************************************/
class GFXcanvasSerialDemo : public GFXcanvas8 {
public:
  GFXcanvasSerialDemo(uint16_t w, uint16_t h);

  /**********************************************************************/
  /*!
    @brief    Prints the current contents of the canvas to Serial
    @param    rotated  true to print according to the current GFX rotation,
    false to print to the native rotation of the canvas (or unrotated).
  */
  /**********************************************************************/
  void print(bool rotated);
};

#endif // __GFXcanvasSerialDemo__
