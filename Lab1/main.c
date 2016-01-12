// INCLUDES
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

// Main

int main(int argc, char* argv[])
{
  
  static const struct option long_opts[] =
    {
      {"rdonly",   required_argument,   0, 'a'},
      {"wronly",   required_argument,   0, 'b'},
      {"command",  no_argument,         0, 'c'},
      {"verbose",  no_argument,         0, 'd'},
      {0,0,0,0},
    };

  
  int option_index = 0;
  int iarg = 0;
  int verbose_flag = 0;
  int exit_status = 0;
  int ret;
  
  while (iarg != -1)
    {
      iarg = getopt_long(argc, argv, "a:b:c:d", long_opts, &option_index);

      if (iarg == -1)
	break;

      if (verbose_flag == 1)
	printf("--%s %s\n", long_opts[iarg-'a'].name, optarg);

      if (iarg == '?')
	continue;
      
      // - is standard input
      // fileno
      
      switch (iarg)
	{
	case 'a':
	  ret = open(optarg, O_RDONLY);
	  if (ret < 0)
	    fprintf(stderr, "File error\n");
	  break;
	case 'b':
	  ret = open(optarg, O_WRONLY);
	  if (ret < 0)
	    fprintf(stderr, "File error\n");
	  break;
	case 'c':
	  pid_t cPID = fork();
	  if (argc - optind < 4)
	    fprintf(stderr, "File descriptor error\n");
	  
	  int fdi = argv[optind++];
	  int fdo = argv[optind++];
	  int fde = argv[optind++];
	  char *command = argv[optind++];
      	  if (cPID >= 0)
	    {
	      if (cPID == 0) // Child process
		{
		  execvp(command, argv[optind]); // Put in the command in first, args in second
		}
	    }
	  else
	    {
	      fprintf(stderr, "Could not create child process");
	      // fork failed
	    }
	  break;
	case 'd':
	  verbose_flag = 1;
	  break;
	default:
	  // No default case required, caught by the ? above switch
	}
    }
  return 0;
}

// CLOSE THE FILES
