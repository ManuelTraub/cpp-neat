/**
 * smal definition of an variable length Array
 * with an max size and an custum content
 *
 * Its just like an Generic ArrayList from Java
 * exclusively the maximum size
 */
#ifndef __DYNAMIC_ARRAY_H__
#define __DYNAMIC_ARRAY_H__
#include <time.h>
#include <sys/types.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Generates an Long 64 Bit random number
 */
#define LONG_RAND                           \
  ((((((((((uint64_t) rand()) << 15)        \
  + (uint64_t) rand()) << 15)               \
  + (uint64_t) rand()) << 15)               \
  + (uint64_t) rand()) << 15)               \
  + (uint64_t) rand() + 1) 

/**
 * Generates an random index for the given Array
 */
#define RAND_INDEX(ARY) LONG_RAND % (ARY).length

/**
 * defines an new Array struct with a given type as content
 * and a given Name
 */
#define DEFINE_ARRAY(TYPE, NAME)          \
typedef struct {                          \
  TYPE *ptr;                              \
  uint64_t length;                        \
  uint64_t max_len;                       \
} NAME

/**
 * Shorter Macro
 */
#define DEF_ARY(TYPE, NAME) DEFINE_ARRAY(TYPE, NAME)

/**
 * Accesses the Element at the given possition
 */
#define ARY_AT(ARY, INDEX) (ARY).ptr[INDEX]

/**
 * returns the element at the given possition
 * (avoids out of range acces)
 */
#define ARY_GET(ARY, INDEX) \
 (((INDEX) >= (ARY).length) ? (decltype(*(ARY).ptr)) 0 : ((ARY).ptr[INDEX]))

/**
 * returns the pointer to the given index
 */
#define ARY_PTR(ARY, INDEX) ((ARY).ptr + (INDEX))

/**
 * returns the last Element of the given ARY
 */
#define ARY_LAST(ARY) ARY_AT(ARY, (ARY).length - 1)

/**
 * Initialize an given Array with an given start length
 * also initialize rand
 *
 * for backward compatibility defining muti arg macro
 */
#define ARY_INIT3(TYPE, ARY, START_LEN)                               \
  do {                                                                \
    (ARY).ptr     = (TYPE *) malloc(sizeof(TYPE) * (START_LEN));      \
    (ARY).length  = 0;                                                \
    (ARY).max_len = START_LEN;                                        \
    srand(time(NULL) * rand());                                       \
  } while (0)

#define ARY_INIT2(ARY, START_LEN) \
  ARY_INIT3(decltype(*(ARY).ptr), ARY, START_LEN)

#define ARY_INIT1(ARY) ARY_INIT2(ARY, 16)

#define ARY_INITX(ARY, T1, T2, T3, FUNC, ...) FUNC
#define ARY_INIT(...) ARY_INITX(, ##__VA_ARGS__,            \
                                  ARY_INIT3(__VA_ARGS__),   \
                                  ARY_INIT2(__VA_ARGS__),   \
                                  ARY_INIT1(__VA_ARGS__))
  

/**
 * Insert an given element at the end of a given Array 
 *
 * Note it my reallocates space if array is to smal
 *
 * NOTE: the Array content type should be the same as the type of 
 *       the given element!
 */
#define ARY_ADD(ARY, E)                                                   \
  do {                                                                    \
    if ((ARY).length >= (ARY).max_len) {                                  \
      (ARY).ptr = (decltype((ARY).ptr)) realloc((ARY).ptr,                \
                        (ARY).length * 2 * sizeof((ARY).ptr[0]));         \
      (ARY).max_len = (ARY).length * 2;                                   \
    }                                                                     \
    (ARY).ptr[(ARY).length] = E;                                          \
    (ARY).length++;                                                       \
  } while (0)

/**
 * Stors the given element at the given possition in the array
 *
 * Note it my reallocates space if array is to smal
 *
 * NOTE: the Array content type should be the same as the type of 
 *       the given element!
 */
#define ARY_SET(ARY, E, I)                                                \
  do {                                                                    \
    if ((I) >= (ARY).max_len) {                                           \
      while ((I) >= (ARY).max_len)                                        \
        (ARY).max_len *= 2;                                               \
                                                                          \
      decltype((ARY).ptr) _ptr = (decltype((ARY).ptr))                    \
        calloc(sizeof((ARY).ptr[0]), (ARY).max_len);                      \
                                                                          \
      if ((_ptr == NULL)                                                  \
        perror("ARY_SET malloc failed!");                                 \
                                                                          \
      memcpy(_ptr, (ARY).ptr, sizeof((ARY).ptr[0]) * ARY_LEN(ARY));       \
      free((ARY).ptr);                                                    \
      (ARY).ptr = _ptr;                                                   \
    }                                                                     \
    (ARY).ptr[I] = E;                                                     \
    (ARY).length = (I) + 1;                                               \
  } while (0)


/* other name */
#define ARY_PUSH(ARY, E) ARY_ADD(ARY, E)


/**
 * grows if neccesary 
 * (expecting you want to store an Element at Index)
 */
#define ARY_GROW(ARY, INDEX)                                                \
  do {                                                                      \
    while ((INDEX) >= (ARY).max_len) {                                      \
      (ARY).ptr = (decltype((ARY).ptr)) realloc((ARY).ptr,                  \
                           (ARY).max_len * 2 * sizeof((ARY).ptr[0]) );      \
      (ARY).max_len *= 2;                                                   \
    }                                                                       \
  } while (0)

  
/**
 * Removes and stores the last element from ARY in E
 */
#define ARY_PULL(ARY, E)                                                  \
  do {                                                                    \
    if ((ARY).length > 0) {                                               \
      (ARY).length--;                                                     \
      E = (ARY).ptr[(ARY).length];                                        \
    } else {                                                              \
      printf("[DEBUG] no more elements in Array, in %s at Line %d\n",     \
             __FILE__, __LINE__);                                         \
      exit(1);                                                            \
    }                                                                     \
  } while (0)

/**
 * Extract a random element from a given array
 * and saves it into a given value if possible
 *
 * note it changes the order of the given array
 * NOTE: the Arry content type should be the same as th type of 
 *       the given value
 */
#define ARY_EXTRACT(ARY, VALUE)                                           \
  do {                                                                    \
    if ((ARY).length > 0) {                                               \
      uint64_t ary_ext_i = RAND_INDEX(ARY);                               \
      VALUE = (ARY).ptr[ary_ext_i];                                       \
      (ARY).ptr[ary_ext_i] = (ARY).ptr[(ARY).length - 1];                 \
      (ARY).length--;                                                     \
    } else {                                                              \
      printf("[DEBUG] no more elements in Array, in %s at Line %d\n",     \
             __FILE__, __LINE__);                                         \
      exit(1);                                                            \
    }                                                                     \
  } while (0)


/**
 * Deletes all instaces of the given Element from the given Array
 * NOTE the Arry content type should be the same as th type of 
 * the given element
 */
#define ARY_DELETE(ARY, E)                                          \
  do {                                                              \
    uint64_t adi, new_len;                                          \
    for (adi = new_len = 0; adi < (ARY).length; adi++) {            \
      (ARY).ptr[new_len] = (ARY).ptr[adi];                          \
      if ((ARY).ptr[adi] != (E)) { new_len++; }                     \
    }                                                               \
    (ARY).length = new_len;                                         \
  } while (0)

/**
 * Deletes the element at the given Position from the given Array
 * NOTE the Arry it changes the order of the given Array!!!
 */
#define ARY_DELETE_AT_FAST(ARY, I)                                  \
  do {                                                              \
    ARY_AT(ARY, I) = ARY_LAST(ARY);                                 \
    (ARY).length--;                                                 \
  } while (0)

/**
 * Deletes the element at the given Position from the given Array
 */
#define ARY_DELETE_AT(ARY, I)                                       \
  do {                                                              \
    memmove((ARY).ptr + (I),                                        \
            (ARY).ptr + (I) + 1,                                    \
            (ARY).length - (I) - 1);                                \
                                                                    \
    (ARY).length--;                                                 \
  } while (0)

/**
 * frees an given ARY
 */
#define ARY_FREE(ARY)         \
do {                          \
  if ((ARY).ptr != NULL) {    \
    free((ARY).ptr);          \
    (ARY).ptr = NULL;         \
  }                           \
} while (0)

/**
 * Returns wheter the given Array is empty or not
 */
#define EMPTY(ARY) ((ARY).length == 0)
#define NOTEMPTY(ARY) ((ARY).length != 0)

/**
 * Return the Length of the given ARRAY
 */
#define ARY_LEN(ARY) ((ARY).length)

/**
 * Switches two Elements at the given indexes
 */
#define ARY_SWITCH(ARY, I, J)               \
  do {                                      \
    ARY_PUSH(ARY, ARY_AT(ARY, I));          \
    ARY_AT(ARY, I) = ARY_AT(ARY, J);        \
    ARY_PULL(ARY, ARY_AT(ARY, J));          \
  } while (0)

/**
 * Puts an given Array into revers order
 */
#define ARY_REVERSE(ARY)                    \
  do {                                      \
    uint64_t len = ARY_LEN(ARY) - 1;        \
    uint64_t i, j = len;                    \
    for (i = 0; i < j; i++, j--)            \
      ARY_SWITCH(ARY, i, j);                \
  } while (0)

/**
 * Clears an given ARY
 * Note it doesn't frees anything it just sets the length to zero
 */
#define ARY_CLEAR(ARY) (ARY).length = 0

/**
 * sets all array indexes to zero
 */
#define ARY_SET_NULL(ARY)                                                     \
  memset((ARY).ptr, 0, sizeof((ARY).ptr[0] * (ARY).length))

/**
 * Clones an given Array
 */
#define ARY_CLONE(DST, SRC)                                                   \
  do {                                                                        \
    ARY_GROW(DST, (SRC).length);                                              \
    memcpy((DST).ptr,                                                         \
           (SRC).ptr,                                                         \
           sizeof((SRC).ptr[0]) * (SRC).length);                              \
    (DST).length = (SRC).length;                                              \
  } while (0)


/**
 * returns wether the given array is NOT empty
 */
#define ARY_NOT_EMPTY(ARY) ((ARY).length != 0)

/**
 * returns wether the given array is empty
 */
#define ARY_EMPTY(ARY) ((ARY).length == 0)

/**
 * insert an element at the given position in the array
 */
#define ARY_INSERT(ARY, I, E)                                                 \
do {                                                                          \
  if (ARY_LEN(ARY) >= (I))                                                    \
    ARY_ADD(ARY, E);                                                          \
  else {                                                                      \
    ARY_GROW(ARY, ARY_LEN(ARY));                                              \
    memmove((ARY).ptr + (I) + 1, (ARY).ptr + (I), ARY_LEN(ARY) - (I));        \
    (ARY).ptr[I] = E;                                                         \
  }                                                                           \
} while (0)


#endif // __DYNAMIC_ARRAY_H__
