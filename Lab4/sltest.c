// Part 2 

/*                                                                                                                                                                                                                  
insert() {                                                                                                                                                                                                         
  critical section here (Put yield inside this critical section)                                                                                                                                                   
    segmentation faults may happen                                                                                                                                                                                 
                                                                                                                                                                                                                  
char [] size 3/4/5 for key???????????                                                                                                                                                                               
                                                                                                                                                                                                                   
elements[number of threads][number of iterations]                                                                                                                                                                  
elements[0][anything] all the stuff for thread 0                                                                                                                                                                   
elements[1][anything] all the stuff for thread 1                                                                                                                                                                   
...                                                                                                                                                                                                                
                                                                                                                                                                                                                  
for(num_iterations)                                                                                                                                                                                                
insert(key);                                                                                                                                                                                                       
                                                                                                                                                                                                                   
list length                                                                                                                                                                                                        
                                                                                                                                                                                                                   
for (num_iterations)                                                                                                                                                                                               
lookup(key)                                                                                                                                                                                                        
delete(key)                                                                                                                                                                                                        
                                                                                                                                                                                                                   
Computation in spec is wrong                                                                                                                                                                                       
10 threads x 1000 iterations x (insert + lookup/del) = 20000 operations                                                                                                                                            
elapsed time: 22177039 ns                                                                                                                                                                                          
per operations: 1108 ns                                                                                                                                                                                                                                                                                                                                                                                                              
*/




#define _GNU_SOURCE
#include "SortedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>

int opt_yield = 0;

void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
  SortedListElement_t *p = list; // Initialized at list head
  SortedListElement_t *n = list->next; // The first element

  while(n != list)
    {
      if (strcmp(element->key, n->key) <= 0)
	break;

      p = n;
      n = n->next;
    }

  element->prev = p;
  element->next = n;

  if (opt_yield && INSERT_YIELD)
    pthread_yield();
  
  p->next = element;
  n->prev = element;
}

int SortedList_delete( SortedListElement_t *element) {
  if (element->key == NULL || element->next->prev != element || element->prev->next != element)
    return 1;
  element->prev->next = element->next;

  if (opt_yield && DELETE_YIELD)
    pthread_yield();
  
  element->next->prev = element->prev;
  // free everything at the end
  return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
  list = list->next;
  while(1) {
    if(list->key == NULL) {
      return NULL;
    }
    if(strcmp(list->key, key) == 0) {
      if (opt_yield && SEARCH_YIELD)
	pthread_yield();
      return list;
    }
  }
  return NULL; //This should never happen
}

int SortedList_length(SortedList_t *list) {
  list = list->next;
  int count = 0;
  while(list->key != NULL) {
    if (list->prev->next != list || list->next->prev != list)
      return -1;
    count++;
    if (opt_yield && SEARCH_YIELD)
      pthread_yield();
    list = list->next;
  }
  return count;
}



/* ----------------- DRIVER ---------------- */


int num_iterations = 1;
int num_threads = 1;

struct thread_arg {
  int id;
};

char sync = '\0';
SortedList_t *list;
SortedListElement_t **element;

void threadFunc(struct thread_arg * arg) {
  int i;
  for(i = 0; i < num_iterations; i++)
    SortedList_insert(list, &element[arg->id][i]);

  SortedList_length(list);
  
  for (i = 0; i < num_iterations; i++) {
    SortedList_lookup(list, element[arg->id][i].key);
    SortedList_delete(&element[arg->id][i]);
  }
}

int main(int argc, char* argv[]) {
  srand(time(NULL));
  int i;
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
	if (optarg == '\0') {
	  fprintf(stderr, "Yield requires arguments\n");
	  exit(1);
	}
	i = 0;
	while (optarg[i] != '\0' && i < 3) {
	  if (optarg[i] == 'i')
	    opt_yield &= INSERT_YIELD;
	  else if (optarg[i] == 'd')
	    opt_yield &= DELETE_YIELD;
	  else if (optarg[i] == 's')
	    opt_yield &= SEARCH_YIELD;
	  else {
	    fprintf(stderr, "Yield requires arguments of i, d, and/or s ONLY\n");
	    exit(1);
	  }
	  i++;
	}
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

  printf("Options finished\n");
  
  element = (SortedListElement_t **)malloc(num_threads * sizeof(SortedListElement_t *));
  int j, k;
  for (i = 0; i < num_threads; i++) {
    element[i] = (SortedListElement_t *)malloc(num_iterations * sizeof(SortedListElement_t));
    for (j = 0; j < num_iterations; j++) {
      element[i][j].next = NULL;
      element[i][j].prev = NULL;
      char *key = (char *) malloc(6 * sizeof(char));
      for (k = 0; k < 5; k++)
	key[k] = (char) rand() % 255 + 1;
      key[5] = '\0';
      element[i][j].key = key;
    }
  }

  struct timespec sTime;
  int start = clock_gettime(CLOCK_MONOTONIC, &sTime);

  pthread_t threads[num_threads];
  

  for (i = 0; i < num_threads; i++) {
    //  fprintf(stdout, "creating %d\n", i);
    struct thread_arg* arg;
    arg->id = i;
    int ret = pthread_create(&threads[i], NULL, (void*)threadFunc, arg);
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
  printf("%d Threads x %d iterations x (ins + lookup/del) = %d operations\n", num_threads, num_iterations, num_ops);
  
  int actual_length = SortedList_length(list);
  if (actual_length != 0)
    fprintf(stderr, "ERROR: final length = %d\n", actual_length);
  

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
