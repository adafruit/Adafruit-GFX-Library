#ifndef __fxp_math_h
#define __fxp_math_h

#include <stdio.h>
#include <inttypes.h>
#include <math.h>

/// Fixed point math

// Approximation factors for atan2, precalculated in Q1.15:
// 0.273 / M_PI * 32768
#define Q15_FACTOR_1_PI 2847
// (0.25 + 0.273 / M_PI) * 32768
#define Q15_FACTOR_1_PI_1_4 11039


/**************************************************************************/
/*!
 * @brief Convert Q1.15 to floating point
 *  @param a Q1.15 fixed point number
 *  @return floating point number
 */
/**************************************************************************/
static inline double q15_to_double(const int16_t a) {
  return (double)a / 32768;
}

/**************************************************************************/
/*!
 * @brief Q1.15 fixed point conversion 
 *  @param d floating point number in range [-1 .. 1 - 2^-15]
 *  @return representation in Q1.15 fixed point format
 */
/**************************************************************************/
static inline int16_t double_to_q15(const double d) {
  return lrint(d * 32768);
}

/**************************************************************************/
/*!
 * @brief Q15 fixed point multiplication with correction 
 *  @param a Q1.15 fixed point number
 *  @param b Q1.15 fixed point number
 *  @return result of multiplication in Q1.15
*/
/**************************************************************************/
static inline int16_t q15_mul(const int16_t a, const int16_t b) {
  const int32_t temp = a * (int32_t)b;
  // Round up and divide by 15
  return (temp + ((temp & 0x7FFF) == 0x4000 ? 0 : 0x4000)) >> 15;
}


/**************************************************************************/
/*!
 * @brief Q15 fixed point division
 *  @param a Q1.15 fixed point number
 *  @param b Q1.15 fixed point number
 *  @return result of division in Q1.15
*/
/**************************************************************************/
static inline int16_t q15_div(const int16_t a, const int16_t b) {
    int32_t temp = (int32_t)a << 15;
    // Round by comparing most significant bits
    if (((temp >> 31) & 1) == ((b >> 15) & 1)) {   
        temp += (b >> 2);
    } else {
        temp -= (b >> 2);
    }
    return (int16_t)(temp / b);
}

/**************************************************************************/
/*!
 * @brief Negative absolute value of a 16bit signed integer
 *  @param a Q1.15 fixed point number
 *  @param b Q1.15 fixed point number
 *  @return result of multiplication in Q1.15
*/
/**************************************************************************/
static inline int16_t i16_nabs(const int16_t x) {
  return x < 0 ? x : -x;
}

uint16_t q15_atan2(const int16_t y, const int16_t x);
int16_t q15_sin(const int16_t a);
int16_t q15_cos(const int16_t a);
int16_t q15_sqrt(const int16_t a);

#endif