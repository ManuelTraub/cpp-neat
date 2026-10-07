#include "utils.h"
#include <inttypes.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
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

rand64_t *new_rand64_t() {
    static rand64_t baseRand = [] {
        const char *seed = getenv("NEAT_SEED");
        if (seed == NULL)
            return gettime_usec();

        rand64_t value = 0;
        bool invalid = *seed == '\0';
        for (const char *p = seed; *p != '\0'; p++) {
            if (*p < '0' || *p > '9' || value > (UINT64_MAX - (*p - '0')) / 10) {
                invalid = true;
                break;
            }
            value = value * 10 + (*p - '0');
        }
        if (invalid || value == 0) {
            fprintf(stderr, "Invalid NEAT_SEED: expected a nonzero unsigned 64-bit decimal value.\n");
            exit(EXIT_FAILURE);
        }
        return value;
    }();

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
