#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
 int status;
 char b[4];
 pid_t pid, ch_pid;
 
 switch(pid=fork()) {
 case -1:
 	perror("Fork failed");
   	exit(1);
  case  0:
   	printf("\nCHILD: This is child process!\n");
   	printf("CHILD: My PID is-- %d\n", getpid());
   	printf("CHILD: My parent PID -- %d\n", getppid());
	printf("CHILD: My GID is -- %d\n", getpgid(getpid()));
	printf("CHILD: My SID is -- %d\n", getsid(getpid()));
	int k = setpgid(getpid(),getpid()); 
/*Modifies group id. Therefore, when user press
		     Cn+C, ChPr can't die*/
	printf("BEFORE SETPGRP CHILD: My GID is -- %d\n", getpgid(getpid()));
	printf("BEFORE SETPGRP CHILD: My SID is -- %d\n", getsid(getpid()));
	read(0,b,4);
	perror("ERROR:");
	//printf("b: %s\n",b);
	pause();
   	exit(0);
  default:
   	printf("PARENT: This is parent process!\n");
        printf("PARENT: My PID -- %d\n", getpid());
        printf("PARENT: My child PID %d\n",pid);
   	printf("PARENT: My parent PID %d\n",getppid());
	printf("PARENT: My GID %d\n",getpgid(getpid()));
	printf("PARENT: My SID %d\n",getsid(getpid()));
	pause();
   	exit(0);
 }
 	return 0;
}
   
