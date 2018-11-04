#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
 int status;
 int a=0,b=0,c=6;
 pid_t pid, ch_pid;
 
 switch(pid=fork()) {
 case -1:
 	perror("Fork failed");
   	exit(1);
  case  0:
   	printf("\nCHILD: This is child process!\n");
   	printf("CHILD: My PID is-- %d\n", getpid());
   	printf("CHILD: My parent PID -- %d\n", getppid());
   	exit(0);
  default:
   	printf("PARENT: This is parent process!\n");
        printf("PARENT: My PID -- %d\n", getpid());
        printf("PARENT: My child PID %d\n",pid);
   	printf("PARENT: My parent PID %d\n",getppid());
	printf("PARENT: I'm waiting for the completion of the child process.\n");
   	ch_pid = wait(&status);
   	printf("PARENT: Child proccess %d finished\n",ch_pid);
   	if (WIFEXITED(status))
            printf("Child exited with code %d\n",WIFEXITED(status));
        else
        printf("Child terminated abnormally\n");
   	exit(0);
 }
 	return 0;
}
