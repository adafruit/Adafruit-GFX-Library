/*
 This a library for fixed point math to implement trigonometic functions
 */

#include "fxp_math.h"

/**************************************************************************/
/*!
 * @brief 16-bit fixed point atan2 approximation
 * Thanks to Xo Wang
 * https://geekshavefeelings.com/posts/fixed-point-atan2
 *
 *  @param y y-coordinate in signed 16-bit
 *  @param x x-coordinate in signed 16-bit
 *  @return angle in (val / 32768) * pi radian increments from 0x0000 to 0xFFFF
 */
/**************************************************************************/
uint16_t q15_atan2(const int16_t y, const int16_t x) {
  if (x == y) {
    // need to handle special cases which cannot represented in Q15.1
    if (y > 0) { // PI/4
      return 8192;
    } else if (y < 0) { // -3/4*PI
      return 40960;
    } else { // y == x == 0
      return 0;
    }
  }
  const int16_t nabs_y = i16_nabs(y), nabs_x = i16_nabs(x);
  if (nabs_x < nabs_y) { // octants 1, 4, 5, 8
    const int16_t y_over_x = q15_div(y, x);
    const int16_t correction = q15_mul(Q15_FACTOR_1_PI, i16_nabs(y_over_x));
    const int16_t unrotated =
        q15_mul(Q15_FACTOR_1_PI_1_4 + correction, y_over_x);
    if (x > 0) { // octants 1, 8
      return unrotated;
    } else { // octants 4, 5
      return 32768 + unrotated;
    }
  } else { // octants 2, 3, 6, 7
    const int16_t x_over_y = q15_div(x, y);
    const int16_t correction = q15_mul(Q15_FACTOR_1_PI, i16_nabs(x_over_y));
    const int16_t unrotated =
        q15_mul(Q15_FACTOR_1_PI_1_4 + correction, x_over_y);
    if (y > 0) { // octants 2, 3
      return 16384 - unrotated;
    } else if (y == -1) {
      return -unrotated;
    } else { // octants 6, 7
      return 49152 - unrotated;
    }
  }
}

/**************************************************************************/
/*!
 * @brief Square root with the babylonian method: xn_1 = 1/2*(x_n + a/x_n)
 *  @param a Q1.15 value between 0 .. 1 - 2^15
 *  @return Q1.15 sqrt between 0 .. 1 - 2^15
 */
/**************************************************************************/
int16_t q15_sqrt(const int16_t a) {
  if (a <= 0)
    return 0; // all cases less or equal 0 are treated as 0
  int16_t xn = 0;
  int16_t xn_1 = a;
  do {
    xn = xn_1;
    int32_t temp = (int32_t)a << 15;
    // Round by comparing most significant bits
    if (((temp >> 31) & 1) == ((xn >> 15) & 1)) {
      temp += (xn >> 2);
    } else {
      temp -= (xn >> 2);
    }
    int32_t a_xn = (xn + temp / xn) << 14;
    // Round up and divide by 15
    xn_1 = (a_xn + ((a_xn & 0x7FFF) == 0x4000 ? 0 : 0x4000)) >> 15;
  } while (xn_1 != xn); // the result will converge
  return xn_1;
}

/**************************************************************************/
/*!
 * @brief 16-bit fixed point sinus based on Bhaskara I' approximation
 *  sin(x) = 16 * x * (PI - x)/(40500 - x * (PI - x))
 *  With a transformation into the Q1.15 space the formula becomes simple.
 *  @param a angle in (val / 32768) * pi radian increments from 0x0000 to 0xFFFF
 *  @return Q1.15 encoded sin(x)
 */
/**************************************************************************/
int16_t q15_sin(const int16_t a) {
  if (a == 0)
    return 0; // special case
  if (a == 16384)
    return 32767; // correct to max value
  // for a positive number > 0
  const int16_t q15_f1 = 32768 - a;
  // multiple by 16/32768
  const int16_t x = q15_mul(a, q15_f1); 
  // multiple by 16/32768
  const int16_t y =
      double_to_q15(5.0 / 16) - q15_mul(double_to_q15(0.25), x);
  const int16_t sin = q15_div(x, y);
  return (a < 0) ? -sin : sin;
}

/**************************************************************************/
/*!
 * @brief 16-bit fixed point cosinus based on Bhaskara I's approximation
 *
 *  @param a angle in (val / 32768) * pi radian increments from 0x0000 to 0xFFFF
 *  @return Q1.15 encoded cos(x)
 */
/**************************************************************************/
int16_t q15_cos(const int16_t a) { return q15_sin(a + 16384); }