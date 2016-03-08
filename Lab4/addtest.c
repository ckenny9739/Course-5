// Part 1 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <pthread.h>

// Global variables
long long count = 0;
int num_iterations = 1;
int num_threads = 1;
pthread_mutex_t test_mutex;
int test_lock = 0;
int opt_yield = 0;
char sync = '\0';

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
  for (i = 0; i < num_iterations; i++) {
    if(sync == 'm') {
      pthread_mutex_lock(&test_mutex);
    }
    else if(sync == 's') {
      while(__sync_lock_test_and_set(&test_lock, 1))
	continue;
    }
    else if(sync == 'c') {
      while(__sync_bool_compare_and_swap(&test_lock, 0, 1) == 0)
	continue;
    }
    
    add(&count, 1);

    if(sync == 'm') {
      pthread_mutex_unlock(&test_mutex);
    }
    else if(sync == 's') {
      __sync_lock_release(&test_lock);
    }
    else if(sync == 'c') {
      __sync_bool_compare_and_swap(&test_lock, 1, 0);
    }
  }

   
  for (i = 0; i < num_iterations; i++) {
    if(sync == 'm') 
      pthread_mutex_lock(&test_mutex);
    
    else if(sync == 's') 
      while(__sync_lock_test_and_set(&test_lock, 1))
	continue;
    
    else if(sync == 'c') 
      while(__sync_bool_compare_and_swap(&test_lock, 0, 1) == 0)
	continue;
    
    add(&count, -1);
    
    if(sync == 'm')
      pthread_mutex_unlock(&test_mutex);
    else if(sync == 's') 
      __sync_lock_release(&test_lock);
    else if(sync == 'c') 
      __sync_bool_compare_and_swap(&test_lock, 1, 0);
  }
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
  if (argc > 5) {
    fprintf(stderr, "Incorrect options\n");
    exit(1);
  }
  while (1) {
    struct option long_options[] =
      {
	{"threads", required_argument, 0, 'a'},
	{"iterations", required_argument, 0, 'b'},
	{"yield", required_argument, 0, 'c'},
	{"sync", required_argument, 0, 'd'},
	{0, 0, 0, 0}
      };
    int option_index = 0;
    int c = getopt_long(argc, argv, "a:b:c:d:", long_options, &option_index);

    if (c == -1)
      break;
    char in;
    int ret;
    switch(c) {
      case 'a':
	ret = (int) strtol(optarg, NULL, 10);
	if (ret <= 0) {
	  fprintf(stderr, "Number of threads must be positive\n");
	  exit(1);
	}
	num_threads = ret;
	break;
      case 'b':
	ret = (int) strtol(optarg, NULL, 10);
	if (ret <= 0) {
	  fprintf(stderr, "Number of iterations must be positive\n");
	  exit(1);
	}
	num_iterations = ret;
	break;
      case 'c':
	ret = (int) strtol(optarg, NULL, 10);
	if (ret != 1) {
	  fprintf(stderr, "Yield option argument must be 1\n");
	  exit(1);
	}
	opt_yield = 1;
	break;
      case 'd':
        in = *optarg;
	if(in == 's' || in == 'm' || in == 'c') {
	  sync = *optarg;
	}
	else {
	  fprintf(stderr, "Incorrect sync options\n");
	  exit(1);
	}
	break;
      default:
	fprintf(stderr, "Incorrect options\n");
	exit(1);
    }
  }
  pthread_mutex_init(&test_mutex, NULL);
  
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


  unsigned long long ns = eTime.tv_nsec - sTime.tv_nsec;
  int carry = 0;
  if(ns < 0) {
    ns += 1000000000;
    carry = 1;
  }
  unsigned long long sec = eTime.tv_sec - sTime.tv_sec - carry;
  double elapsed = sec*1000000000 + ns;
  fprintf(stdout, "elapsed time: %.0f ns\n", elapsed);

  fprintf(stdout, "per operation: %f ns\n", elapsed/num_ops);

  exit(0);
}

  // time is 3485788ms so divide by 200k, time per op is like 1.7 ns ???? 17 ms
 

// Suggested to use 1, 2, 10 threads |||||||| 20, 1000 iterations
// Not enough so increase? 1 - 10 threads, 1 / 10 / 50 / 100 / 200 / 500 / 1000 / 2000 / 5000 / 10000
