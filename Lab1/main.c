// INCLUDES
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

int check_size(int* arr, int arr_ind, int *arr_size)
{
  if (arr_ind >= (*arr_size))
    {
      (*arr_size) = (*arr_size) * 2;
      arr = (int*) realloc(arr, (*arr_size));
    }
  if (!arr)
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
      {"command",  required_argument,   0, 'c'},
      {"verbose",  no_argument,         0, 'd'},
      {0,0,0,0},
    };
  
  int option_index = 0;
  int iarg = 0;
  int verbose_flag = 0;
  int fd_ind = 0;
  int fd_size = 128;
  int* file_descriptors = (int*) malloc(fd_size*sizeof(int));
  int fdi, fdo, fde;
  int ret;
  pid_t cPID;
  int arg_ind = 0;
  int arg_size = 512;
  char **args = (char**) malloc(arg_size*sizeof(char));
  int i;
  int j = 0;
  
  while (iarg != -1)
    {
      j = 0;
      //printf("argv has %s\n", argv[optind]);
      iarg = getopt_long(argc, argv, "a:b:c:d", long_opts, &option_index);
      //printf("iarg : %d\n", iarg);
      if (iarg == -1)
	break;
      
      if (long_opts[iarg-'a'].has_arg == required_argument)
	optind--;

      if (verbose_flag == 1) {
	printf("--%s", long_opts[iarg-'a'].name);
	for (i = optind; i < argc; i++)
	  {
	    if (strlen(argv[i]) > 2 && (argv[i][0] == '-' && argv[i][1] == '-'))
	      break;
	    printf(" %s", argv[i]);
	  }
	printf("\n");
      }

      if (iarg == '?')
	continue;
      
      switch (iarg)
	{
	case 'a':
	  ret = check_size(file_descriptors, fd_ind, (&fd_size));
	  if (ret == -1) {
	    fprintf(stderr, "Memory allocation error for file descriptors\n");
	    break;
	  }
	  ret = open(optarg, O_RDONLY);
	  if (ret < 0) {
	    fprintf(stderr, "File error\n");
	    break;
	  }
	  file_descriptors[fd_ind++] = ret;
	  //  printf("file_descriptors[fd_ind++] (for read) : %d\n", ret);
	  //  printf("Finished read\n");
	  break;
	  
	case 'b':
	  ret = check_size(file_descriptors, fd_ind, (&fd_size));
	  if (ret == -1) {
	    fprintf(stderr, "Memory allocation error for file descriptors\n");
	    break;
	  }
	  ret = open(optarg, O_WRONLY);
	  if (ret < 0) {
	    fprintf(stderr, "File error\n");
	    break;
	  }
	  file_descriptors[fd_ind++] = ret;
	  //  printf("file_descriptors[fd_ind++] (for write) : %d\n", ret);
	  //  printf("Finished write\n");
	  break;
	  
	case 'c':
	  if(argc - optind < 4) {
	    fprintf(stderr, "Not enough arguments\n");
	    break;
	  }
	  if (sscanf (argv[optind++], "%i", &fdi) != 1) {
	    fprintf(stderr, "Not a file descriptor\n");
	    break;
	  }

	  if (sscanf (argv[optind++], "%i", &fdo) != 1) {
	    fprintf(stderr, "Not a file descriptor\n");
	    break;
	  }

	  if (sscanf (argv[optind++], "%i", &fde) != 1) {
	    fprintf(stderr, "Not a file descriptor\n");
	    break;
	  }
	  
	  // printf("optind is pointing to %s\n", argv[optind]);
	  //printf("optind is pointing to %s\n", argv[optind]);
	  //printf("optind is pointing to %s\n", argv[optind]);
	  //printf("optind is pointing to %s\n", argv[optind]);
	  for (i = optind; i < argc; i++)
	    {
	      if (strlen(argv[i]) > 2 && (argv[i][0] == '-' && argv[i][1] == '-'))
		break;
	      args[j++] = argv[i];
	      //printf("optind is pointing to %s\n", argv[optind]);
	      optind++;
	    }
	  args[j] = NULL;
	  if (fdi > fd_ind || fdo > fd_ind || fde > fd_ind) {
	    fprintf(stderr, "Invalid file descriptor\n");
	    break;
	  }
	  cPID = fork();
	  //  printf("Finished fork\n");
	  if (cPID >= 0)
	    {
	      if (cPID == 0) // Child process
		{
		  //printf("Child Process: %s\n", args[0]);
		  if (dup2(file_descriptors[fdi], 0) == -1) {
		    fprintf(stderr, "Invalid file descriptor - input");
		    break;
		  }
		  if (dup2(file_descriptors[fdo], 1) == -1) {
		    fprintf(stderr, "Invalid file descriptor - output");
		    break;
		  }
		  if (dup2(file_descriptors[fde], 2) == -1) {
		    fprintf(stderr, "Invalid file descriptor - error");
		    break;
		  }
		  //  printf("Round 2: input: %d, output: %d, error: %d\n", fdi, fdo, fde);
		  //  printf("Command: %s\n", args[0]);
		  //  printf("Arguments: %s\n", args[0]);
		  //  printf("Arguments: %s\n", args[1]);
		  //  printf("Arguments: %s\n", args[2]);
		  execvp(args[0], args);
		  // Put in the command name in first, args (including args[0] as name) in second
		  // Error check for error in execvp
		  fprintf(stderr, "execvp error\n");
		  exit(1); // Something else here????
		}
	   }
	  else
	    {
	      fprintf(stderr, "Could not create child process");
	      // fork failed
	    }
	  //	  printf("Thread - %d Finished command\n", cPID);
	  break;
	case 'd':
	  verbose_flag = 1;
	  //	  printf("Finished verbose\n");
	  break;
	 // No default case required, caught by the ? above switch
	}
    }
  //printf("Main returned\n");
  return 0;
}
