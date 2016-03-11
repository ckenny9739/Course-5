// Part 2 

#include "SortedList.h"
#include <time.h>
#include <stdlib.h>


void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
  srand(time(NULL));
  element->key = malloc(sizeof(char) * 5);
  int i;
  for(i = 0; i < 4; i++) {
    element->key[i] = (char) rand() % 255 + 1;
  }
  element->key[4] = '\0';
  
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
  p->next = element;
  n->prev = element;
  }
}

int SortedList_delete( SortedListElement_t *element) {
  element->prev = element->next;
  element->next = element->prev;
  free(key);
  free(element);
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
  list = list->next;
  while(1) {
    if(list->key == NULL) {
      return NULL;
    }
    if(strcmp(list->key, key) == 0) {
      return list;
    }
  }
  return NULL; //This should never happen
}

int SortedList_length(SortedList_t *list) {
  list = list->next;
  int count = 1;
  while(list->key != NULL) {
    count++;
    list = list->next;
  }
  return count;
}


int num_iterations = 1;
int num_threads = 1;

void thread_func() {
  int i;
  for(i = 0; i < num_iterations; i++)
    //insert(key);

    //list length

    for (i = 0; i < num_iterations; i++) {
      //lookup(key)
      //	delete(key)
    }
}

int main(int argc, char **argv) {
  

}


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
