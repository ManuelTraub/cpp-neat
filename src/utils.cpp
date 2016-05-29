#include "utils.h"
#include <inttypes.h>
#include <cmath>
#include <string.h>
#include <string>
#include <stdlib.h>
using std::string;

uint32_t rand32(uint32_t *x) {
    *x ^= *x << 13;
    *x ^= *x >> 17;
    *x ^= *x << 5;
    return *x;
}

rand64_t *new_rand64_t(uint32_t seed) {
    rand64_t *rand = (rand64_t *) malloc(sizeof(rand64_t));
    *rand = ((uint64_t) rand32(&seed)) | (((uint64_t) rand32(&seed)) << 32);
    return rand;
}

uint64_t rand64(rand64_t *rand) {
  *rand ^= *rand << 13;
  *rand ^= *rand >> 7;
  *rand ^= *rand << 17;
  return *rand;
}

/**
 * converts a given integer to a std::string
 */
std::string itoa(int64_t i) {

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
std::string ftoa(double d, unsigned precision) {
    
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

/**
 * Returns if the given args conatinig the given arg
 */
bool has_arg(int argc, 
             char *argv[], 
             const char *short_arg, 
             const char *long_arg) {

  int i;
  for (i = 1; i < argc; i++) {
    if ((short_arg != NULL && !strcmp(argv[i], short_arg)) ||
        (long_arg != NULL  && !strcmp(argv[i], long_arg))) {
      
      return true;
    } else if (strlen(argv[i]) == 2 && !strcmp(argv[i], "--"))
      return false;
  }

  return false;
}

/**
 * Returns the given argument arg
 */
string get_arg(int argc, 
               char *argv[], 
               const char *short_arg, 
               const char *long_arg) {

  int i;
  for (i = 1; i < argc - 1; i++) {
    if ((short_arg != NULL && !strcmp(argv[i], short_arg)) ||
        (long_arg != NULL  && !strcmp(argv[i], long_arg))) {
      
      return string(argv[i + 1]);
    } else if (strlen(argv[i]) == 2 && !strcmp(argv[i], "--"))
      return string("");
  }

  return string("");
}

/**
 * Returns the given argument index
 */
int get_arg_i(int argc, 
              char *argv[], 
              const char *short_arg, 
              const char *long_arg) {

  int i;
  for (i = 1; i < argc - 1; i++) {
    if ((short_arg != NULL && !strcmp(argv[i], short_arg)) ||
        (long_arg != NULL  && !strcmp(argv[i], long_arg))) {
      
      return i + 1;
    } else if (strlen(argv[i]) == 2 && !strcmp(argv[i], "--"))
      return -1;
  }

  return -1;
}
