#ifndef __UTILS_H__
#define __UTILS_H__

#include <inttypes.h>
#include <cmath>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <vector>

using std::string;
using std::vector;

/* returns the current milliseconds time */
uint64_t gettime_usec();

/* random value */
typedef uint64_t rand64_t; 

rand64_t *new_rand64_t();

uint64_t rand64(rand64_t *rand);

/**
 * converts a given integer to a std::string
 */
std::string itoa(int64_t i);

#define rand_range64(rand, start, end) (start + (rand64(rand) % (((end) + 1) - (start))))
#define rand_range_d64(rand, start, end) \
  (((double(end) - double(start)) * rand64(rand) + double(start) * 18446744073709551616.0) / 18446744073709551616.0)

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

#ifndef NDEBUG
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
#endif /* NDEBUG */

/* returns the absolute difference between the two numbers 
 * (circumevents integer overflows) 
 */
#define absDiff(x, y) (((x) > (y)) ? ((x) - (y)) : ((y) - (x)))

/* returns the middle beweten the two given values
 * (circumevents integer overflows) 
 */
#define middle(x, y) (((x) < (y)) ? ((x) + ((y) - (x)) / 2) : ((y) + ((x) - (y)) / 2))

/* signum function */
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}
/**
 * Splits a String by a specific char
 * returns vector<string> splitted Strings
 */
std::vector<std::string> split(const string str, const string seperator);


#endif /* __UTILS_H__ */
