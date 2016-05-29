#ifndef __UTILS_H__
#define __UTILS_H__

#include <inttypes.h>
#include <cmath>
#include <string.h>
#include <string>
#include <stdlib.h>
using std::string;

uint32_t rand32(uint32_t *x);

/* random value */
typedef uint64_t rand64_t; 

rand64_t *new_rand64_t(uint32_t seed);

uint64_t rand64(rand64_t *rand);

/**
 * converts a given integer to a std::string
 */
std::string itoa(int64_t i);

/**
 * converts a given double to a std::string
 */
std::string ftoa(double d, unsigned precision);

#define rand_range64(rand, start, end) (start + (rand64(rand) % (((end) + 1) - (start))))
#define rand_range_d64(rand, start, end) \
  (((end - start) * rand64(rand) + start * 18446744073709551616.0) / 18446744073709551616.0)

/**
 * Returns if the given args conatinig the given arg
 */
bool has_arg(int argc, 
             char *argv[], 
             const char *short_arg, 
             const char *long_arg);

/**
 * Returns the given argument arg
 */
string get_arg(int argc, 
               char *argv[], 
               const char *short_arg, 
               const char *long_arg);

/**
 * Returns the given argument index
 */
int get_arg_i(int argc, 
              char *argv[], 
              const char *short_arg, 
              const char *long_arg);

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
