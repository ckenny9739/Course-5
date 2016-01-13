// INCLUDES
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

int check_size(int* file_descriptors, int fd_ind, int fd_size)
{
  if (fd_ind >= fd_size)
    {
      fd_size = fd_size * 2;
      file_descriptors = (int*) realloc(file_descriptors, fd_size);
    }
  if (!file_descriptors)
    return 1; //Returns 1 on error
  return 0;
}

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
  int fd_ind = 0;
  int fd_size = 128;
  int* file_descriptors = (int*) malloc(128*sizeof(int));
  int ret;
  pid_t cPID;
  int fdi, fdo, fde;
  char *args[512]; // limitation?
  int i;
  int j = 0;

  // KEEP TRACK OF FILE DESCRIPTORS
  
  while (iarg != -1)
    {
      iarg = getopt_long(argc, argv, "a:b:c:d", long_opts, &option_index);
      printf("iarg : %d\n", iarg);
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
	  ret = check_size(file_descriptors, fd_ind, fd_size);
	  if (ret == -1)
	    fprintf(stderr, "Memory allocation error for file descriptors\n");
	  else
	    fd_size = fd_size * 2;
	  ret = open(optarg, O_RDONLY);
	  if (ret < 0)
	    fprintf(stderr, "File error\n");
	  file_descriptors[ret] = ret;
	  fd_ind++;
	  printf("Finished read\n");
	  break;
	  
	case 'b':
	  ret = check_size(file_descriptors, fd_ind, fd_size);
	  if (ret == -1)
	    fprintf(stderr, "Memory allocation error for file descriptors\n");
	  else
	    fd_size = fd_size * 2;
	  ret = open(optarg, O_WRONLY);
	  if (ret < 0)
	    fprintf(stderr, "File error\n");
	  file_descriptors[ret] = ret;
	  fd_ind++;
	  printf("Finished write\n");
	  break;
	  
	case 'c':
	  fdi = atoi(argv[optind++]);
	  fdo = atoi(argv[optind++]);
	  fde = atoi(argv[optind++]);
	  printf("Check 1\n");
	  for (i = optind; i < argc; i++)
	    {
	      if (argv[i][0] == '-' && argv[i][1] == '-')
		break;
	      args[j++] = argv[i];
	    }
	  if (file_descriptors[fdi] != fdi || file_descriptors[fdo] != fdo || file_descriptors[fde] != fde)
	    fprintf(stderr, "Invalid file descriptor\n");
	  printf("input: %d, output: %d, error: %d\n", fdi, fdo, fde);
	  printf("Made it to before forking stuff\n");
	  cPID = fork();
	  printf("Finished fork\n");
	  if (cPID >= 0)
	    {
	      if (cPID == 0) // Child process
		{
		  printf("Child Process\n");
		  if (dup2(fdi, STDIN_FILENO) == -1)
		    fprintf(stderr, "Invalid file descriptor - input");
		  if (dup2(fdo, STDOUT_FILENO) == -1)
		    fprintf(stderr, "Invalid file descriptor - output");
		  if (dup2(fde, STDERR_FILENO) == -1)
		    fprintf(stderr, "Invalid file descriptor - error");
		  //      		  close(fdi);
		  //		  close(fdo);
		  //close(fde);
		  printf("Round 2: input: %d, output: %d, error: %d\n", fdi, fdo, fde);
		  printf("Command: %s\n", args[0]);
		  printf("Arguments: %s\n", args[0]);
		  printf("Arguments: %s\n", args[1]);
		  printf("Arguments: %s\n", args[2]);
		  execvp(args[0], args); // Put in the command name in first, args (including args[0] as name) in second
		  // Error check for error in execvp
		  printf("execvp returned....\n");
		  exit(1); // Something else here????
		}
	   }
	  else
	    {
	      fprintf(stderr, "Could not create child process");
	      // fork failed
	    }
	  printf("Thread - %d Finished command\n", cPID);
	  break;
	case 'd':
	  verbose_flag = 1;
	  printf("Finished verbose\n");
	  break;
	default:
	  printf("Default case????\n");
	 // No default case required, caught by the ? above switch
	}
    }
  return 0;
}
