#include <stdio.h>
int main() {
  FILE *pipe = popen("gnuplot -persist","w");
  fprintf(pipe, "set data style lines\n");
  fprintf(pipe, "plot 'yourfile.dat' using 1:2\n");
  close(pipe);
}
