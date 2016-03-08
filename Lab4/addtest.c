// Part 1 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <pthread.h>

/*
  1.2
  mutex
  Global --> pthread_mutex_t test_mutex;

  pthread_mutex_init(&test_mutex, NULL);

  pthread_mutex_lock(&test_mutex);
  add(...)
  pthread_mutex_unlock(&test_mutex);


  spinlock

  GLOBAL --> int test_lock = 0;
  while(__sync_lock_test_and_set(&test_lock, 1));
  add();
  __sync_lock_release(&test_lock);


  

 */


// Global variables
long long count = 0;
int num_iterations = 1;
int num_threads = 1;
pthread_mutex_t test_mutex;
int test_lock = 0;
int opt_yield = 0;

void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if(opt_yield) {
    pthread_yield();
  }
  *pointer = sum;
}

void threadFunc() {
  int i;
  //  printf("1 Thread Func: %d\n", num_iterations);
  for (i = 0; i < num_iterations; i++)
    add(&count, 1);

  //  printf("2 Thread Func\n");
  
  for (i = 0; i < num_iterations; i++)
    add(&count, -1);

  //  printf("3 Thread Func\n");
}

/*
collect the start time
create all threads
run all threads
wait all thread to finish
collect the end time
elapsed time = end time - start time
*/
int main(int argc, char* argv[]) {

  // Use options here...
  if (argc > 4) {
    fprintf(stderr, "Incorrect options\n");
    exit(1);
  }
  while (1) {
    struct option long_options[] =
      {
	{"threads", optional_argument, 0, 'a'},
	{"iterations", optional_argument, 0, 'b'},
	{"yield", optional_argument, 0, 'c'},
	{0, 0, 0, 0}
      };
    int option_index = 0;
    int c = getopt_long(argc, argv, "a::b::c::", long_options, &option_index);

    if (c == -1)
      break;

    switch(c) {
      case 'a':
	num_threads = (int) strtol(optarg, NULL, 10);
	break;
      case 'b':
	num_iterations = (int) strtol(optarg, NULL, 10);
	break;
      case 'c':
	opt_yield = (int) strtol(optarg, NULL, 10);
	break;
      default:
	fprintf(stderr, "Incorrect options\n");
	exit(1);
    }
  }
  
  struct timespec sTime;
  int start = clock_gettime(CLOCK_MONOTONIC, &sTime);

  pthread_t threads[num_threads];
  
  int i;
  for (i = 0; i < num_threads; i++) {
    //  fprintf(stdout, "creating %d\n", i);
    int ret = pthread_create(&threads[i], NULL, (void*)threadFunc, NULL);
    if (ret != 0) {
      fprintf(stderr, "Error: failure upon creating thread %d", i);
      exit(ret);
    }
  }

  for ( i = 0; i < num_threads; i++) {
    //  fprintf(stdout, "joining %d\n", i);
    int ret = pthread_join(threads[i], NULL);
    if (ret != 0) {
      fprintf(stderr, "Error: failure upon joining thread %d", i);
      exit(ret);
    }
  }
  //  fprintf(stdout, "finished joining\n");
  
  struct timespec eTime;
  int end = clock_gettime(CLOCK_MONOTONIC, &eTime);



  // print to stdout the number of ops
  int num_ops = num_threads * num_iterations * 2;
  printf("%d Threads x %d iterations x (add + subtract) = %d operations\n", num_threads, num_iterations, num_ops);
  
  if (count != 0) {
    fprintf(stderr, "ERROR: final count = %d\n", count);
  }


  long int ns = eTime.tv_nsec - sTime.tv_nsec;
  int carry = 0;
  if(ns < 0) {
    ns += 1000000000;
    carry = 1;
  }
  long int sec = eTime.tv_sec - sTime.tv_sec - carry;
  fprintf(stdout, "elapsed time: %d ns\n", sec*1000000000 + ns);

  fprintf(stdout, "per operation: %f ns\n", (double)(sec*1000000000 + ns)/num_ops);

  exit(0);
}

  // time is 3485788ms so divide by 200k, time per op is like 1.7 ns ???? 17 ms
 

// Suggested to use 1, 2, 10 threads |||||||| 20, 1000 iterations
// Not enough so increase? 1 - 10 threads, 1 / 10 / 50 / 100 / 200 / 500 / 1000 / 2000 / 5000 / 10000
