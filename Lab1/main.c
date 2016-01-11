// INCLUDES
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

// Main

int main(int argc, char* argv[])
{
  
  const static struct option long_opts[] =
    {
      {"rdonly",   required_argument,   0, 'a'},
      {"wronly",   required_argument,   0, 'b'},
      {"command",  required_argument,   0, 'c'},
      {"verbose",  no_argument,         0, 'd'},
      {0,0,0,0},
    };

  
  int option_index = 0;
  int iarg = 0;
  int verbose_flag = 0;
  
  while (iarg != -1)
    {
      iarg = getopt_long(argc, argv, "a:b:c:d", long_opts, &option_index);

      if (iarg == -1)
	break;

      if (iarg == '?')
	continue;

      if (verbose_flag == 1)
	{
	  // Check which is right later
	  //	  printf("%s", argv[optind-1]);
	  printf("--%s %s\n", long_opts[iarg-'a'].name, optarg);
	}
      
      // - is standard input
      // fileno
      
      switch (iarg)
	{
	case 'a':
	  // Error check all of these
	  open(optarg, O_RDONLY);
	  break;
	case 'b':
	  open(optarg, O_WRONLY);
	  break;
	case 'c':
	  break;
	case 'd':
	  verbose_flag = 1;
	  break;
	default:
	  fprintf(stderr, "Invalid option - %d", iarg);
	  exit(1);
	  break;
	}
    }

}


// CLOSE THE FILES
