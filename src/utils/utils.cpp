#include "utils.h"
#include <inttypes.h>
#include <cmath>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <sys/time.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
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

/* converts a given time to a string */
std::string formatTimestamp(uint64_t time_usec) {
    // Convert microseconds to system clock time point
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::time_point(std::chrono::microseconds(time_usec));

    // Convert system clock time point to time_t for breaking down into components
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm* gmt = std::gmtime(&tt);  // Convert time_t to tm struct as UTC time

    // Extracting milliseconds from input (microseconds)
    int milliseconds = (time_usec % 1000000) / 1000;

    // Formatting the string
    std::stringstream ss;
    ss << std::put_time(gmt, "%Y-%m-%d-%H-%M-%S");  // Format date and time
    ss << "-" << std::setfill('0') << std::setw(3) << milliseconds;  // Format milliseconds

    return ss.str();
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


/**
 * Splits a String by a specific char
 * returns vector<string> splitted Strings
 */
vector<string> split(const string str, const string seperator) {

  char *copy = strdup(str.c_str());
  char *ptr = strtok(copy, seperator.c_str());

  vector<string> res;

  int i;
  for (i = 0; ptr != NULL; i++) {

    int size = strlen(ptr);
    res.push_back(string(ptr, size));
    ptr = strtok(NULL, seperator.c_str());
  }

  free(copy);
  return res;
}

/* returns true wether the given string ends with the given substring */
bool endsWith(std::string str, std::string substr) {
    if (str.length() < substr.length()) 
        return false;

    const int offset = str.length() - substr.length();
    for (unsigned i = offset; i < str.length(); i++)
        if (str[i] != substr[i - offset])
            return false;

    return true;
}

