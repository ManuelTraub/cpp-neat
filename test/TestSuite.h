/**
 *  Simple testing framework
 */
#ifndef __TEST_H__
#define __TEST_H__

#include "dynamic-array.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/**
 * The overall function parameter gerenrated in bevor_all, 
 * used in bevor and destroyed in after_all
 * define if you unhappy with void *
 */
#ifndef static_t
#define static_t void
#endif


/**
 * The overall function parameter for bevor, after, after_all, and the 
 * different test cases define if you unhappy with void *
 */
#ifndef test_t
#define test_t void
#endif

/**
 * A simple test case
 */
typedef struct {
  test_t *(*func) (test_t *); /* the test function */
  int    runs;                /* the number of test runs */
  char   failed;              /* indecates if this test failed */
  char   skipped;             /* indecates if tis test was skipped */
  const  char *test_name;     /* the name of the test */
  struct timeval start;
  struct timeval end;
  struct timeval time;
} Test;

/**
 * function pointer array for the tests
 */
DEF_ARY(Test, TestAry);

/**
 * The TestSuite structur
 *
 * +-------------+-----------------------------------------------------------+
 * | bevor_all   | the function is the first function ever called,           |
 * |             | its return value is passed to all bevor functions.        |
 * |             |                                                           |
 * | bevor       | this function is called bevor every test, it gets the     |
 * |             | return value from, the bevor_all function,                |
 * |             | the retun value from this will the be given to the        |
 * |             | test function                                             |
 * |             |                                                           |
 * | after       | this function gets the return value from the previous     |
 * |             | executed test function, and should process clean up after |
 * |             | a test.                                                   |
 * |             | Note: if the test failed it gets the pointer form bevor.  |
 * |             |                                                           |
 * | after_all   | This function is executed bevor the exit of the test, it  |
 * |             | gets the return value from bevor_all, and soud do a last  |
 * |             | clean up.                                                 |
 * |             |                                                           |
 * | verbose     | print output to stdout, standart is true.                 |
 * |             |                                                           |
 * | log         | logs the output to a log file, standtart is false.        |
 * +-------------+-----------------------------------------------------------+
 *
 * Note: all the given argumnets ar named args, so dont use that name if you
 *       dont use the args
 */                                                                         
typedef struct {
  const char *fname;
  static_t *(*bevor_all) (void);
  test_t   *(*bevor) (static_t *);
  void      (*after) (test_t *);
  void      (*after_all) (static_t *);
  static_t *bevor_all_res;
  TestAry   tests;
  char      verbose;
  char      exit_on_error;
  char      log;
  FILE     *log_fd;
  uint64_t  i;      /* index of the current progressing test */
  struct timeval start;
  struct timeval end;
  struct timeval time;
} TestSuite;

/**
 * the Test Suite
 */
static TestSuite tunit;

/**
 * macro for adding the bevor_all function
 */
#define BEVOR_ALL()                                                         \
/* declaration */                                                           \
static_t *bevor_all(void);                                                  \
                                                                            \
/* add the function to the test unit */                                     \
void __attribute__((constructor(102))) add_bevor_all() {                    \
  tunit.bevor_all = bevor_all;                                              \
}                                                                           \
                                                                            \
/* start of the function implementation */                                  \
static_t *bevor_all(void)


/**
 * macro for adding the bevor function
 */
#define BEVOR()                                                             \
/* declaration */                                                           \
test_t *bevor(static_t *args);                                              \
                                                                            \
/* add the function to the test unit */                                     \
void __attribute__((constructor(102))) add_bevor() {                        \
  tunit.bevor = bevor;                                                      \
}                                                                           \
                                                                            \
/* start of the function implementation */                                  \
test_t *bevor(static_t *args)


/**
 * macro for adding the after function
 */ 
#define AFTER()                                                             \
/* declaration */                                                           \
void after(test_t *args);                                                   \
                                                                            \
/* add the function to the test unit */                                     \
void __attribute__((constructor(102))) add_after() {                        \
  tunit.after = after;                                                      \
}                                                                           \
                                                                            \
/* start of the function implementation */                                  \
void after(test_t *args)


/**
 * macro for adding the after_all function
 */
#define AFTER_ALL()                                                         \
/* declaration */                                                           \
void after_all(static_t *args);                                             \
                                                                            \
/* add the function to the test unit */                                     \
void __attribute__((constructor(102))) add_after_all() {                    \
  tunit.after_all = after_all;                                              \
}                                                                           \
                                                                            \
/* start of the function implementation */                                  \
void after_all(static_t *args)



/**
 * macro for adding a test case with a given number of runs
 */
#define TEST_N(NAME, N)                                                     \
/* declaration */                                                           \
test_t *NAME(test_t *args);                                                 \
                                                                            \
/**                                                                         \
 * add the function to the test unit                                        \
 */                                                                         \
void __attribute__ ((constructor(102))) add_test_##NAME() {                 \
                                                                            \
  Test test;                                                                \
  memset(&test, 0, sizeof(Test));                                           \
  test.func = NAME;                                                         \
  test.runs = N;                                                            \
  test.test_name = #NAME;                                                   \
                                                                            \
  ARY_ADD(tunit.tests, test);                                               \
}                                                                           \
                                                                            \
/* start of the function implementation */                                  \
test_t *NAME(test_t *args)

/**
 * default test with 1 run
 */
#define TEST(NAME) TEST_N(NAME, 1)

/* macro for filename without path */
#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

/**
 * print or log a message
 * useable like printf
 */
#define TEST_MSGS(...)                                        \
do {                                                          \
                                                              \
  if (tunit.verbose) {                                        \
    fprintf(stdout, "[SS] " __VA_ARGS__);                     \
    fprintf(stdout, "\n");                                    \
  }                                                           \
                                                              \
  if (tunit.log) {                                            \
    fprintf(tunit.log_fd, "[SS] " __VA_ARGS__);               \
    fprintf(tunit.log_fd, "\n");                              \
  }                                                           \
                                                              \
} while (0)

/**
 * print or log a message
 * useable like printf
 */
#define TEST_MSGE(...)                                        \
do {                                                          \
                                                              \
  if (tunit.verbose > 1) {                                    \
    fprintf(stdout,                                           \
            "[EE] [%s:%d]",                                   \
            __FILENAME__,                                     \
            __LINE__);                                        \
    fprintf(stdout, __VA_ARGS__);                             \
    fprintf(stdout, "\n");                                    \
  }                                                           \
                                                              \
  if (tunit.log) {                                            \
    fprintf(tunit.log_fd,                                     \
            "[EE] [%s:%d]",                                   \
            __FILENAME__,                                     \
            __LINE__);                                        \
    fprintf(tunit.log_fd, __VA_ARGS__);                       \
    fprintf(tunit.log_fd, "\n");                              \
  }                                                           \
                                                              \
} while (0)

/**
 * print or log a message
 * useable like printf
 */
#define TEST_MSGW(...)                                        \
do {                                                          \
                                                              \
  if (tunit.verbose > 2) {                                    \
    fprintf(stdout,                                           \
            "[WW] [%s:%d]",                                   \
            __FILENAME__,                                     \
            __LINE__);                                        \
    fprintf(stdout, __VA_ARGS__);                             \
    fprintf(stdout, "\n");                                    \
  }                                                           \
                                                              \
  if (tunit.log) {                                            \
    fprintf(tunit.log_fd,                                     \
            "[WW] [%s:%d]",                                   \
            __FILENAME__,                                     \
            __LINE__);                                        \
    fprintf(tunit.log_fd, __VA_ARGS__);                       \
    fprintf(tunit.log_fd, "\n");                              \
  }                                                           \
                                                              \
} while (0)

/**
 * print or log a message
 * useable like printf
 */
#define TEST_MSGI(...)                                        \
do {                                                          \
                                                              \
  if (tunit.verbose > 3) {                                    \
    fprintf(stdout, "[II] " __VA_ARGS__);                     \
    fprintf(stdout, "\n");                                    \
  }                                                           \
                                                              \
  if (tunit.log) {                                            \
    fprintf(tunit.log_fd, "[II] " __VA_ARGS__);               \
    fprintf(tunit.log_fd, "\n");                              \
  }                                                           \
                                                              \
} while (0)

/**
 * print or log a message
 * useable like printf
 */
#define TEST_MSGD(...)                                        \
do {                                                          \
                                                              \
  if (tunit.verbose > 4) {                                    \
    fprintf(stdout, "[DD] " __VA_ARGS__);                     \
    fprintf(stdout, "\n");                                    \
  }                                                           \
                                                              \
  if (tunit.log) {                                            \
    fprintf(tunit.log_fd, "[DD] " __VA_ARGS__);               \
    fprintf(tunit.log_fd, "\n");                              \
  }                                                           \
                                                              \
} while (0)

/**
 * Skip the current test
 */
#define SKIP_TEST(...)                                        \
do {                                                          \
                                                              \
  if (tunit.verbose > 2) {                                    \
    fprintf(stdout,                                           \
            "[WW] [%s:%d] skipping %s",                       \
            __FILENAME__,                                     \
            __LINE__,                                         \
            ARY_AT(tunit.tests, tunit.i).test_name);          \
    fprintf(stdout, __VA_ARGS__);                             \
    fprintf(stdout, "\n");                                    \
  }                                                           \
                                                              \
  if (tunit.log) {                                            \
    fprintf(tunit.log_fd,                                     \
            "[WW] [%s:%d] skipping %s",                       \
            __FILENAME__,                                     \
            __LINE__,                                         \
            ARY_AT(tunit.tests, tunit.i).test_name);          \
    fprintf(tunit.log_fd, __VA_ARGS__);                       \
    fprintf(tunit.log_fd, "\n");                              \
  }                                                           \
                                                              \
  CUR_TEST_SKIPPED = 1;                                       \
                                                              \
  /* return the args given by bevor */                        \
  return args;                                                \
} while (0);


/**
 * assert that the condition is false 
 * if its true it logs or prints a stacktrace and 
 * if given formated message
 */
#define ASSERT_FALSE(CONDITION, ...)                          \
if (CONDITION) {                                              \
                                                              \
  if (tunit.verbose > 1) {                                    \
    fprintf(stdout,                                           \
            "[EE] [%s:%d] in %s ",                            \
            __FILENAME__,                                     \
            __LINE__,                                         \
            ARY_AT(tunit.tests, tunit.i).test_name);          \
    fprintf(stdout, "assertion failed: " __VA_ARGS__);        \
    fprintf(stdout, "\n");                                    \
  }                                                           \
                                                              \
  if (tunit.log) {                                            \
    fprintf(tunit.log_fd,                                     \
            "[EE] [%s:%d] in %s ",                            \
            __FILENAME__,                                     \
            __LINE__,                                         \
            ARY_AT(tunit.tests, tunit.i).test_name);          \
    fprintf(tunit.log_fd, "assertion failed: " __VA_ARGS__);  \
    fprintf(tunit.log_fd, "\n");                              \
  }                                                           \
                                                              \
  /* let test fail */                                         \
  ARY_AT(tunit.tests, tunit.i).failed = 1;                    \
                                                              \
  /* return the args given by bevor */                        \
  return args;                                                \
}                                       

/**
 * assert that the condition is false 
 * if its true it logs or prints a stacktrace and 
 * if given formated message
 */
#define ASSERT_TRUE(CONDITION, ...) ASSERT_FALSE(!(CONDITION), __VA_ARGS__)

/**
 * assert that the two conditins are equal 
 */
#define ASSERT_EQUAL(COND1, COND2, ...) ASSERT_FALSE((COND1) != (COND2), __VA_ARGS__);

/**
 * let the test fail with a backtrace and a given formated message
 */
#define ASSERT_FAIL(...) ASSERT_FALSE(1, __VA_ARGS__)


/*********************************************************************
 * include main and source                                           *
 * (we have to do this to use the same static tunit with in test.c   *
 *  and within the actual test source file)                          *
 *********************************************************************/

/**
 * init tunit first;
 */
void __attribute__ ((constructor(101))) init_tunit() {

  memset(&tunit, 0, sizeof(TestSuite));
  ARY_INIT(Test, tunit.tests, 8);
}

/**
 * Macro to acces the current running test
 */
#define CUR_TEST ARY_AT(tunit.tests, tunit.i)

/**
 * returns if the current test failed
 */
#define CUR_TEST_FAILED CUR_TEST.failed

/**
 * returns if the current test was skipped
 */
#define CUR_TEST_SKIPPED CUR_TEST.skipped

/**
 * print usage
 */
static inline void print_usage(char *name) {
  printf("%s Test Suite:\n", name);
  printf("  -h  --help\t\tprint these message\n"
         "  -q  --quiet\t\tbe quiet\n"
         "  -l  --log [FILE]\tlog qutput to FILE\n"
         "  -e  --exit-on-error\texit on assertion fail\n"
         "  -v  --verbose [1,2,3,4,5] set the verbose level\n"
         "        1: only summary\n"
         "        2: 1 + errors\n"
         "        3: 2 + warnings\n"
         "        4: 3 + one-line test info (default)\n"
         "        5: 4 + extendet test info\n");
}

/**
 * prints a smal start up message 
 */
static inline void info_messages(int argc, char *argv[]) {
  
  TEST_MSGD("Test Framework started with %lu tests", 
            ARY_LEN(tunit.tests));

  if (tunit.log)
    TEST_MSGD("logging to %s", get_arg(argc, argv, "-l", "--log").c_str());
}

/**
 * initializes the Test Framework
 */
static inline void init(int argc, char *argv[]) {

  /* save file name */
#ifdef _WIN32
  tunit.fname = strrchr(argv[0], '\\') ? strrchr(argv[0], '\\') + 1 : argv[0];
#else
  tunit.fname = strrchr(argv[0], '/') ? strrchr(argv[0], '/') + 1 : argv[0];
#endif

  /* standart verbose level */
  tunit.verbose = 4;

  if (has_arg("-h", "--help")) {
    print_usage(argv[0]);
    exit(EXIT_SUCCESS);
  }

  if (has_arg("-v", "--verbose"))
    tunit.verbose = atoi(get_arg("-v", "--verbose").c_str());
  
  tunit.verbose       = has_arg("-q", "--quiet") ? 0 : tunit.verbose;
  tunit.log           = has_arg("-l", "--log");
  tunit.exit_on_error = has_arg("-e", "--exit-on-error");

  if (tunit.log)
    tunit.log_fd = fopen(get_arg("-l", "--log").c_str(), "w");

  if (tunit.log_fd == NULL && tunit.log) {
    perror("Failed to open log file");
    exit(EXIT_FAILURE);
  }

  info_messages(argc, argv);
}

/**
 * runns the current test
 * and exits (the child prozess)
 */
static inline void run_test(Test *test) {

  test_t *bevor_res = NULL;
  test_t *test_res  = NULL;
  
  /* run bevor with result of bevor_all */
  if (tunit.bevor != NULL)
    bevor_res = tunit.bevor(tunit.bevor_all_res);
  
  /* run test */
  test_res = test->func((tunit.bevor ? bevor_res : (test_t *) tunit.bevor_all_res));

  /* do clean up */
  if (tunit.after != NULL)
    tunit.after(test_res);

  if (test->failed && tunit.exit_on_error)
      exit(EXIT_FAILURE);
}

/**
 * prints and or logs information about the current finished test
 */
static inline void do_test_info(Test *test) {
  
  const char *exit_status = test->failed ? "with failur" : "succesfull";

  double seconds = (double) test->time.tv_sec + 
                   (double) test->time.tv_usec / 1000000.0L;

  TEST_MSGD("Test %s needed %0.5f seconds, exited %s",
           test->test_name,
           seconds,
           exit_status);
}

/**
 * prints a summery after all test ar runed
 */
static inline void do_test_summary(const char *name) {
  
  unsigned int n_failurs, n_skipped, i;

  for (i = 0, n_failurs = 0, n_skipped = 0;
       i < ARY_LEN(tunit.tests); 
       i++) {

    if (ARY_AT(tunit.tests, i).failed)
      n_failurs++;

    if (ARY_AT(tunit.tests, i).skipped)
      n_skipped++;
  }

  TEST_MSGS("Test Framework %s finished %lu tests",
           name,
           ARY_LEN(tunit.tests));

  TEST_MSGS("Succes: %lu", ARY_LEN(tunit.tests) - n_failurs);
  TEST_MSGS("Skiped: %u", n_skipped);
  TEST_MSGS("Failed: %u", n_failurs);

  if (n_failurs)
      exit(EXIT_FAILURE);
}

/**
 * Return 1 if the difference is negative, otherwise 0.  
 * Saves the Difference of t1 and t2 in result
 */
static inline void timeval_subtract(struct timeval *result, 
                                    struct timeval *t2, 
                                    struct timeval *t1) {

    int64_t diff = (t2->tv_usec + 1000000L * t2->tv_sec) -
                   (t1->tv_usec + 1000000L * t1->tv_sec);

    result->tv_sec  = diff / 1000000L;
    result->tv_usec = diff % 1000000L;
}

/**
 * Main function which runs the tests
 */
int main(int argc, char *argv[]) {

  init(argc, argv);

  /* stop time */
  gettimeofday(&tunit.start, NULL);

  /* run bevor_all function */
  if (tunit.bevor_all != NULL)
    tunit.bevor_all_res = tunit.bevor_all();

  /* run all tests  in the order they where added */
  for (tunit.i = ARY_LEN(tunit.tests) - 1; tunit.i < ARY_LEN(tunit.tests); tunit.i--) {

    Test *test = &CUR_TEST;

    TEST_MSGI("starting test: %s", test->test_name); 
    gettimeofday(&test->start, NULL);

    /* run test */
    for (int r = 0; r < test->runs; r++)
        run_test(test);

    gettimeofday(&test->end, NULL);
    timeval_subtract(&test->time, &test->end, &test->start);

    /* print info about the finished test */
    do_test_info(test);
  }
  
  if (tunit.after_all != NULL)
    tunit.after_all(tunit.bevor_all_res);

  gettimeofday(&tunit.end, NULL);
  timeval_subtract(&tunit.time, &tunit.end, &tunit.start);

  /* print summery */
  do_test_summary(tunit.fname);

  return 0;
}

#endif /* __TEST_H__ */
