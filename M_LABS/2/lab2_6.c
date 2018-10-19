#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>

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
	//prctl(PR_SET_PDEATHSIG, SIGHUP); /*Kill the child process when PP has died*/
	pause(); /*Cntrl+C is killing both process*/
   	exit(0);
  default:
   	printf("PARENT: This is parent process!\n");
        printf("PARENT: My PID -- %d\n", getpid());
        printf("PARENT: My child PID %d\n",pid);
   	printf("PARENT: My parent PID %d\n",getppid());
	pause(); 
	/*kill -9 pid will destroy PP, CP will have init as parent*/
   	exit(0);
 }
 	return 0;
}
   
