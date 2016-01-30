// INCLUDES
#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>


void check_args(char * argv[], int argc, int optind) {
  if (optind + 1 == argc)
    return;
  if (strlen(argv[optind+1]) < 2 || argv[optind+1][0] != '-' || argv[optind+1][1] != '-')
    fprintf(stderr, "Invalid options - Too many arguments for %s\n", argv[optind-1]);
  return;
}

int check_size(void* arr, int arr_ind, unsigned int *arr_size)
{
  if ((unsigned) arr_ind >= (*arr_size))
    {
      (*arr_size) = (*arr_size) * 2;
      arr = (int*) realloc(arr, (*arr_size));
    }
  if (!arr)
    return 1; //Returns 1 on error
  return 0;
}

void simpsh_handler(int signum)
{
  fprintf(stderr, "%d caught\n", signum);
  exit(signum);
}

// Main

int main(int argc, char* argv[])
{
  char **copy = malloc(argc*sizeof(char*));
  int l = 0;
  for(l = 0; l < argc; l++) {
    copy[l] = malloc((strlen(argv[l]) + 1)*sizeof(char));
    strcpy(copy[l], argv[l]);
  }
  
  
  static const struct option long_opts[] =
    {
      {"rdonly",    required_argument,   0, 'a'},
      {"wronly",    required_argument,   0, 'b'},
      {"command",   required_argument,   0, 'c'},
      {"verbose",   no_argument,         0, 'd'},
      {"append",    no_argument,         0, 'e'},
      {"cloexec",   no_argument,         0, 'f'},
      {"creat",     no_argument,         0, 'g'},
      {"directory", no_argument,         0, 'h'},
      {"dsync",     no_argument,         0, 'i'},
      {"excl",      no_argument,         0, 'j'},
      {"nofollow",  no_argument,         0, 'k'},
      {"nonblock",  no_argument,         0, 'l'},
      {"rsync",     no_argument,         0, 'm'},
      {"sync",      no_argument,         0, 'n'},
      {"trunc",     no_argument,         0, 'o'},
      {"rdwr",      required_argument,   0, 'p'},
      {"pipe",      no_argument,         0, 'q'},
      {"abort",     no_argument,         0, 'r'},
      {"close",     required_argument,   0, 's'},
      {"catch",     required_argument,   0, 't'},
      {"ignore",    required_argument,   0, 'u'},
      {"default",   required_argument,   0, 'v'},
      {"pause",     no_argument,         0, 'w'},
      {"wait",      no_argument,         0, 'x'},
      {0,0,0,0},
    };

  int return_value = 0;
  int bit_mask = 0;
  int option_index = 0;
  int iarg = 0;
  int verbose_flag = 0;
  int fd_ind = 0;
  int pCount = 0;
  int waitFlag = 0;
  int arg_ind = 0;
  int i = 0;
  int j = 0;
  int fdi, fdo, fde;
  int ret, stat;
  pid_t cPID;
  int pipe_fd[2];
  unsigned int fd_size = 128;
  int* file_descriptors = (int*) malloc(fd_size*sizeof(int));
  unsigned int arg_size = 512;
  char **args = (char**) malloc(arg_size*sizeof(char*));
  int argvInd;
  
  struct proc {
    pid_t id;
    int start;
    int end;
  };
  unsigned int procSize = 32;
  struct proc *procArr = (struct proc*)malloc(procSize*sizeof(struct proc));
  
  while (iarg != -1)
    {
      j = 0;
      //printf("argv has %s\n", argv[optind]);
      iarg = getopt_long(argc, argv, "a:b:c:defghijklmnop:qrs:t:u:v:wx", long_opts, &option_index);
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

      if (optind != argc && long_opts[iarg-'a'].has_arg == no_argument && \
	  (strlen(argv[optind]) < 2 || argv[optind][0] != '-' || argv[optind][1] != '-')) {
	fprintf(stderr, "Invalid Argument - Option has no argument\n");
      }

      int who = RUSAGE_SELF;
      struct rusage usage;
      ret = getrusage(who, &usage);
      printf("ret: %d\n", ret);
      printf("ru_utime.tv_sec: %ld, ru_utime.tv_usec: %ld\n", (long int) usage.ru_utime.tv_sec, (long int) usage.ru_utime.tv_usec);
      printf("ru_stime.tv_sec: %ld, ru_stime.tv_usec: %ld\n", (long int) usage.ru_stime.tv_sec, (long int) usage.ru_stime.tv_usec);

      for (i = 0; i < 1000000; i++)
	{
	  int x = 2;
	}

      switch (iarg)
	{
	case 'a':
	  ret = check_size(file_descriptors, fd_ind, (&fd_size));
	  if (ret == -1) {
	    fprintf(stderr, "Memory allocation error for file descriptors\n");
	    break;
	  }
	  ret = open(optarg, O_RDONLY | bit_mask, 0644);
	  bit_mask = 0;
	  if (ret < 0) {
	    return_value = 1;
	    fprintf(stderr, "Open failed - Read(%d)\n", ret);
	    break;
	  }
	  file_descriptors[fd_ind++] = ret;
	  check_args(argv, argc, optind);
	  //  printf("file_descriptors[fd_ind++] (for read) : %d\n", ret);
	  //  printf("Finished read\n");
	  break;
	  
	case 'b':
	  ret = check_size(file_descriptors, fd_ind, (&fd_size));
	  if (ret == -1) {
	    fprintf(stderr, "Memory allocation error for file descriptors\n");
	    break;
	  }
	  ret = open(optarg, O_WRONLY | bit_mask, 0644);
	  bit_mask = 0;
	  if (ret < 0) {
	    return_value = 1;
	    fprintf(stderr, "Open failed - Write (%d)\n", ret);
	    break;
	  }
	  file_descriptors[fd_ind++] = ret;
	  check_args(argv, argc, optind);
	  //  printf("file_descriptors[fd_ind++] (for write) : %d\n", ret);
	  //  printf("Finished write\n");
	  break;
	  
	case 'c':
	  if(argc - optind < 4) {
	    fprintf(stderr, "Not enough arguments\n");
	    return_value = 1;
	    break;
	  }
	  
	  if (sscanf (argv[optind++], "%i", &fdi) < 1) {
	    return_value = 1;
	    fprintf(stderr, "Not a file descriptor\n");
	    break;
	  }

	  if (sscanf (argv[optind++], "%i", &fdo) < 1) {
	    return_value = 1;
	    fprintf(stderr, "Not a file descriptor\n");
	    break;
	  }

	  if (sscanf (argv[optind++], "%i", &fde) < 1) {
	    return_value = 1;
	    fprintf(stderr, "Not a file descriptor\n");
	    break;
	  }

	  //printf("optind is pointing to %s\n", argv[optind]);
	  //printf("optind is pointing to %s\n", argv[optind]);
	  //printf("optind is pointing to %s\n", argv[optind]);
	  //printf("optind is pointing to %s\n", argv[optind]);
	  argvInd = optind;
	  //	  printf("Beginning: %s %d\n", argv[argvInd], argvInd);
	  for (i = optind; i < argc; i++)
	    {
	      if (strlen(argv[i]) > 2 && (argv[i][0] == '-' && argv[i][1] == '-'))
		break;
	      while (strlen(argv[i]) >= arg_size)
		check_size(args, arg_ind, &arg_size);
	      args[j++] = argv[i];
	      //printf("optind is pointing to %s\n", argv[optind]);
	      optind++;
	    }
	  args[j] = NULL;
	  if (fdi < 0 || fdi >= fd_ind || fdo < 0 || fdo >= fd_ind || fde < 0 || fde >= fd_ind) {
	    fprintf(stderr, "Invalid file descriptor\n");
	    return_value = 1;
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
		    fprintf(stderr, "Dup2 error - input\n");
		    return_value = 1;
		    break;
		  }
		  if (dup2(file_descriptors[fdo], 1) == -1) {
		    fprintf(stderr, "Dup2 error - output\n");
		    return_value = 1;
		    break;
		  }
		  if (dup2(file_descriptors[fde], 2) == -1) {
		    fprintf(stderr, "Dup2 error - error\n");
		    return_value = 1;
		    break;
		  }
		  // Close File Descriptors if not equal to the correct value
		  for (i = 0; i < fd_ind; i++) {
		    if (i != fdi && i != fdo && i != fde)
		      close(file_descriptors[i]);
		  }
		  
		  // printf("Round 2: input: %d, output: %d, error: %d\n", fdi, fdo, fde);
		  // printf("Command: %s\n", args[0]);
		  // printf("Arguments: %s\n", args[0]);
		  // printf("Arguments: %s\n", args[1]);
		  // printf("Arguments: %s\n", args[2]);
		  // printf("Arguments: %s\n", args[3]);
		  // printf("Execvp running\n");

		  ret = execvp(args[0], args);
		  fprintf(stderr, "Execvp returned - FAILURE\n");
		  // Put in the command name in first, args (including args[0] as name) in second
		  // Error check for error in execvp
		  // fprintf(stderr, "execvp error\n");
		  exit(1); // Exit because 1 failed
		}
	      else {
		check_size(procArr, pCount, &procSize);
		procArr[pCount].id = cPID;
		procArr[pCount].start = argvInd;
		procArr[pCount].end = optind;
		//printf("argv[start] = %s, argv[end] = %s\n", argv[argvInd], argv[optind]);
		pCount++;
	      }
	   }
	  else
	    {
	      fprintf(stderr, "Could not create child process\n");
	      // fork failed
	    }
	  // printf("Thread - %d Finished command\n", cPID);
	  //	  printf("End: %s %d\n", argv[argvInd], argvInd);
	  //printf("End: %s %d\n", argv[optind], optind);
	  break;
	case 'd':
	  verbose_flag = 1;
	  // printf("Finished verbose\n");
	  break;
	case 'e':
	  bit_mask |= O_APPEND;
	  break;
	case 'f':
	  bit_mask |= O_CLOEXEC;
	  break;
	case 'g':
	  bit_mask |= O_CREAT;
	  break;
	case 'h':
	  bit_mask |= O_DIRECTORY;
	  break;
	case 'i':
	  bit_mask |= O_DSYNC;
	  break;
	case 'j':
	  bit_mask |= O_EXCL;
	  break;
	case 'k':
	  bit_mask |= O_NOFOLLOW;
	  break;
	case 'l':
	  bit_mask |= O_NONBLOCK;
	  break;
	case 'm':
	  bit_mask |= O_RSYNC;
	  break;
	case 'n':
	  bit_mask |= O_SYNC;
	  break;
	case 'o':
	  bit_mask |= O_TRUNC;
	  break;
	case 'p':
	  ret = check_size(file_descriptors, fd_ind, (&fd_size));
	  if (ret == -1) {
	    fprintf(stderr, "Memory allocation error for file descriptors\n");
	    return_value = 1;
	    break;
	  }
	  ret = open(optarg, O_RDWR | bit_mask, 0644);
	  bit_mask = 0;
	  if (ret < 0) {
	    fprintf(stderr, "Open failed - Read/Write(%d)\n", ret);
	    return_value = 1;
	    break;
	  }
	  file_descriptors[fd_ind++] = ret;
	  check_args(argv, argc, optind);
	  //  printf("file_descriptors[fd_ind++] (for Read/Write) : %d\n", ret);
	  //  printf("Finished Read/Write\n");
	  break;
	case 'q':
	  // Use + 1 to check for 2 open file descriptors because pipe uses 2
	  ret = check_size(file_descriptors, fd_ind + 1, (&fd_size));
	  if (ret == -1) {
	    fprintf(stderr, "Memory allocation error for file descriptors\n");
	    return_value = 1;
	    break;
	  }
	  ret = pipe(pipe_fd);
	  if (ret < 0) {
	    fprintf(stderr, "Pipe failed - %d\n", ret);
	    return_value = 1;
	    break;
	  }
	  file_descriptors[fd_ind++] = pipe_fd[0];
	  file_descriptors[fd_ind++] = pipe_fd[1];
	  break;
	case 'r':
	  raise(SIGSEGV);
	  break;
	case 's':
	  if(sscanf(optarg, "%i", &ret) < 1 || ret >= fd_ind) {
	    fprintf(stderr, "Error - Not a file descriptor\n");
	    return_value = 1;
	    break;
	  }
	  if (close(file_descriptors[ret]) < 0)
	    fprintf(stderr, "Error closing\n");
	  check_args(argv, argc, optind);
	  break;
	case 't':
	  if(sscanf(optarg, "%i", &ret) < 1) {
	    fprintf(stderr, "Error - Not a signal number\n");
	    return_value = 1;
	    break;
	  }
	  if (signal(ret, simpsh_handler) == SIG_ERR) {
	    return_value = 1;
	    fprintf(stderr, "Error catching\n");
	  }
	  check_args(argv, argc, optind);
	  break;
	case 'u':
	  if(sscanf(optarg, "%i", &ret) < 1) {
	    fprintf(stderr, "Error - Not a signal number\n");
	    return_value = 1;
	    break;
	  }
	  if (signal(ret, SIG_IGN) == SIG_ERR) {
	    fprintf(stderr, "Error ignoring\n");
	    return_value = 1;
	  }
	  check_args(argv, argc, optind);
	  break;
	case 'v':
	  if(sscanf(optarg, "%i", &ret) < 1) {
	    fprintf(stderr, "Error - Not a signal number\n");
	    return_value = 1;
	    break;
	  }
	  if (signal(ret, SIG_DFL) == SIG_ERR) {
	    fprintf(stderr, "Error default\n");
	    return_value = 1;
	  }
	  check_args(argv, argc, optind);
	  break;
	case 'w':
	  pause();
	  break;
	case 'x':
	  waitFlag = 1;
	  break;
	default:
	  fprintf(stderr, "Error - How did you get here?\n");
	  return_value = 1;
	  break;
	}
      ret = getrusage(who, &usage);
      printf("ret: %d\n", ret);
      printf("ru_utime.tv_sec: %ld, ru_utime.tv_usec: %ld\n", (long int) usage.ru_utime.tv_sec, (long int) usage.ru_utime.tv_usec);
      printf("ru_stime.tv_sec: %ld, ru_stime.tv_usec: %ld\n", (long int) usage.ru_stime.tv_sec, (long int) usage.ru_stime.tv_usec);
    }
  //  printf("Main returned\n");
  if(waitFlag) {
    // for(i = 0; i < argc; i++) {
    //  printf("%s %d \n", argv[i], i);
    // }
    for(i=0; i < fd_ind; i++)
      close(file_descriptors[i]);

    for(i=0; i < pCount; i++) {
      ret = wait(&stat);
      if (ret < 0) {
	fprintf(stderr, "Process Failed\n");
	return_value = 1;
      }
      // for(j = 0; j < argc; j++) {
      //printf("%s %d \n", argv[j], j);
      //}
      for(j=0; j < pCount; j++) {
	if (procArr[j].id == ret) {
	  if (WEXITSTATUS(stat) > 0)
	    return_value = return_value > WEXITSTATUS(stat) ? return_value : WEXITSTATUS(stat);
	  //printf("pid: %d, argv[start]: %s, argv[end]: %s\n", procArr[j].id, argv[procArr[j].start], argv[procArr[j].end]);
	  printf("%d", WEXITSTATUS(stat));
	  //  printf("\nWait\n");
	  int p;
	  for(p = procArr[j].start; p != procArr[j].end; p++)
	    printf(" %s", copy[p]);
	  printf("\n");
	  break;
	}
      }
    }
  }
  free(file_descriptors);
  free(args);
//  printf("Main exited with %d\n", return_value);
  exit(return_value);
}
