// Part 2 

#include "SortedList.h"
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
