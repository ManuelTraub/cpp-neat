#ifndef __UTILS_H__
#define __UTILS_H__

#include <inttypes.h>
#include <cmath>
#include <string.h>
#include <string>
#include <stdlib.h>

static inline uint32_t rand32(uint32_t *x) {
  *x ^= *x << 13;
  *x ^= *x >> 17;
  *x ^= *x << 5;
  return *x;
}

/* random value */
typedef struct {
  uint32_t x, y, z, w;
} rand128_t; 

static inline rand128_t *new_rand128_t(uint32_t seed) {
  rand128_t *rand = (rand128_t *) malloc(sizeof(rand128_t));

  rand->x = rand32(&seed);
  rand->y = rand32(&seed);
  rand->z = rand32(&seed);
  rand->w = rand32(&seed);

  return rand;
}

static inline uint32_t rand128(rand128_t *rand) {
                               
  uint32_t t = rand->x ^ (rand->x << 11);
  rand->x = rand->y; rand->y = rand->z; rand->z = rand->w;
  rand->w ^= (rand->w >> 19) ^ t ^ (t >> 8);
                                      
  return rand->w;
}

/**
 * converts a given integer to a std::string
 */
static inline std::string itoa(int64_t i) {

  char a[32];

  if (i == 0) {
    a[0] = '0';
    a[1] = '\0';
    return std::string(a);
  }

  char is_neg = (i < 0);

  if (is_neg)
    i *= -1;

  int64_t j;
  for (j = 0; i != 0; j++, i /= 10)
    a[j] = 48 + (i % 10);

  if (is_neg) {
     a[j] = '-';
    j++;
  }

  a[j] = '\0';
  std::string str = std::string(a);;
  return std::string(str.rbegin(), str.rend());
}

/**
 * converts a given double to a std::string
 */
static inline std::string ftoa(double d, unsigned precision) {
    
  /* check for NaN */
  if (std::isnan(d))
      return std::string("NaN");

  /* check for infinity */
  if (std::isinf(d) && d < 0.0)
      return std::string("-inf");
  if (std::isinf(d) && d > 0.0)
      return std::string("inf");


  std::string res = itoa(d) + ".";

  if (d < 0.0)
    d *= -1.0;

  double decimal = (d - ((double) ((uint64_t) d))) * 10;

  for (unsigned i = 0; i < precision; i++) {
    res += itoa(decimal);
    decimal = decimal - ((double) ((uint64_t) decimal));
    decimal *= 10;
  }

  return res;
}

#define rand_range128(rand, start, end) (start + (rand128(rand) % (((end) + 1) - (start))))
#define rand_range_d128(rand, start, end) \
  (((end - start) * rand128(rand) + start * 4294967295.0) / 4294967295.0)

#endif
