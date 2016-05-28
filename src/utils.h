#ifndef __UTILS_H__
#define __UTILS_H__
#define __DEBUG__

#include <inttypes.h>
#include <cmath>
#include <string.h>
#include <string>
#include <stdlib.h>
using std::string;

static inline uint32_t rand32(uint32_t *x) {
    *x ^= *x << 13;
    *x ^= *x >> 17;
    *x ^= *x << 5;
    return *x;
}

/* random value */
typedef uint64_t rand64_t; 

static inline rand64_t *new_rand64_t(uint32_t seed) {
    rand64_t *rand = (rand64_t *) malloc(sizeof(rand64_t));
    *rand = ((uint64_t) rand32(&seed)) | (((uint64_t) rand32(&seed)) << 32);
    return rand;
}

static inline uint64_t rand64(rand64_t *rand) {
  *rand ^= *rand << 13;
  *rand ^= *rand >> 7;
  *rand ^= *rand << 17;
  return *rand;
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

#define rand_range64(rand, start, end) (start + (rand64(rand) % (((end) + 1) - (start))))
#define rand_range_d64(rand, start, end) \
  (((end - start) * rand64(rand) + start * 4294967295.0) / 4294967295.0)

/**
 * Returns if the given args conatinig the given arg
 */
static bool has_arg(int argc, 
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
static string get_arg(int argc, 
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
static int get_arg_i(int argc, 
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

/**
 * shorter macros to use in main
 */
#define has_arg1(long_arg) has_arg(argc, argv, NULL, long_arg)
#define has_arg2(short_arg, long_arg) has_arg(argc, argv, short_arg, long_arg)
#define has_arg4(argc, argv, short_arg, long_arg) \
  has_arg(argc, argv, short_arg, long_arg)
#define has_argx(X, T1, T2, T3, T4, FUNC, ...) FUNC
#define has_arg(...) has_argx(, ##__VA_ARGS__,            \
                                has_arg4(__VA_ARGS__),    \
                                "3 args not allowed",     \
                                has_arg2(__VA_ARGS__),    \
                                has_arg1(__VA_ARGS__))

#define get_arg1(long_arg) get_arg(argc, argv, NULL, long_arg)
#define get_arg2(short_arg, long_arg) get_arg(argc, argv, short_arg, long_arg)
#define get_arg4(argc, argv, short_arg, long_arg) \
  get_arg(argc, argv, short_arg, long_arg)
#define get_argx(X, T1, T2, T3, T4, FUNC, ...) FUNC
#define get_arg(...) get_argx(, ##__VA_ARGS__,            \
                                get_arg4(__VA_ARGS__),    \
                                "3 args not allowed",     \
                                get_arg2(__VA_ARGS__),    \
                                get_arg1(__VA_ARGS__))

#define get_arg_i1(long_arg) get_arg_i(argc, argv, NULL, long_arg)
#define get_arg_i2(short_arg, long_arg) get_arg_i(argc, argv, short_arg, long_arg)
#define get_arg_i4(argc, argv, short_arg, long_arg) \
  get_arg_i(argc, argv, short_arg, long_arg)
#define get_arg_ix(X, T1, T2, T3, T4, FUNC, ...) FUNC
#define get_arg_i(...) get_arg_ix(, ##__VA_ARGS__,              \
                                  get_arg_i4(__VA_ARGS__),      \
                                  "3 args not allowed",         \
                                  get_arg_i2(__VA_ARGS__),      \
                                  get_arg_i1(__VA_ARGS__))

/**
 * to get integer args
 */
#define get_i_arg(...)                                                    \
  (has_arg(__VA_ARGS__) ? atoi(get_arg(__VA_ARGS__).c_str()) : -1)

#define get_l_arg(...)                                                    \
  (has_arg(__VA_ARGS__) ? atol(get_arg(__VA_ARGS__).c_str() : -1)

#define get_ll_arg(...)                                                   \
  (has_arg(__VA_ARGS__) ? atoll(get_arg(__VA_ARGS__).c_str() : -1)

/**
 * to get float args
 */
#define get_f_arg(...)                                                    \
  (has_arg(__VA_ARGS__) ? atof(get_arg(__VA_ARGS__).c_str()) : -1.0)

#ifdef __DEBUG__
#include <assert.h>
#include <stdio.h>
#define assertf(ex,fmt,...) do {    \
    if (!(ex)) {                    \
        printf(fmt, __VA_ARGS__);   \
        assert(ex);                 \
    }                               \
} while (0)
#else
#define assertf(...)
#endif /* __DEBUG__ */
#endif /* __UTILS_H__ */
