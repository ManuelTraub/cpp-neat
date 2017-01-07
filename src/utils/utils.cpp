#include "utils.h"
#include <inttypes.h>
#include <cmath>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <sys/time.h>
using std::string;

/* returns the current milliseconds time */
uint64_t gettime_usec() {

    struct timeval time;
    int error = gettimeofday(&time, NULL);
    if (error)
        return ((uint64_t) -1);

    return ((uint64_t) time.tv_sec) * ((uint64_t) 1000000) + 
           ((uint64_t) time.tv_usec);
}

uint32_t rand32(uint32_t *x) {
    *x ^= *x << 13;
    *x ^= *x >> 17;
    *x ^= *x << 5;
    return *x;
}

rand64_t *new_rand64_t() {
    static rand64_t baseRand = gettime_usec();

    rand64_t *rand = (rand64_t *) malloc(sizeof(rand64_t));
    *rand = rand64(&baseRand);
    return rand;
}

uint64_t rand64(rand64_t *rand) {
  *rand ^= *rand << 13;
  *rand ^= *rand >> 7;
  *rand ^= *rand << 17;
  return *rand;
}

/**
 * converts a given integer to a string
 */
string itoa(int64_t i) {

  char a[32];

  if (i == 0) {
    a[0] = '0';
    a[1] = '\0';
    return string(a);
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
  string str = string(a);;
  return string(str.rbegin(), str.rend());
}

/**
 * converts a given double to a string
 */
string ftoa(double d, unsigned precision) {
    
  /* check for NaN */
  if (std::isnan(d))
      return string("NaN");

  /* check for infinity */
  if (std::isinf(d) && d < 0.0)
      return string("-inf");
  if (std::isinf(d) && d > 0.0)
      return string("inf");

  string res = (d < 0.0) ? "-" : "";

  if (d < 0.0)
    d *= -1.0;

  res += itoa(d) + ".";

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
                    
/* returns the next bigger prime number */
uint64_t nextPrime(uint64_t n) {
    
    for (bool prime = false; !prime ;n++) {

        if (n % 2 == 0)
            continue;
        
        prime = true;
        const uint64_t max = sqrt(n) + 1;

        for (uint64_t i = 3; i < max; i += 2) {
            if (n % i == 0) {
                prime = false;
                break;
            }
        }
    }

    return n;
}
