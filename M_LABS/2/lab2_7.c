#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
 int status;
 pid_t pid, ch_pid;
 
 switch(pid=fork()) {
 case -1:
 	perror("Fork failed");
   	exit(1);
  case  0:
   	printf("\nCHILD: This is child process!\n");
   	printf("CHILD: My PID is-- %d\n", getpid());
   	printf("CHILD: My parent PID -- %d\n", getppid());
	setpgrp(); /*Modifies group id. Therefore, when user press
		     Cn+C, ChPr can't die*/
	pause();
   	exit(0);
  default:
   	printf("PARENT: This is parent process!\n");
        printf("PARENT: My PID -- %d\n", getpid());
        printf("PARENT: My child PID %d\n",pid);
   	printf("PARENT: My parent PID %d\n",getppid());
	pause();
   	exit(0);
 }
 	return 0;
}
   
