#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
 int a=0,b=0,c=6;
 pid_t pid, ch_pid;
 static struct sigaction paract, oact;
 void handler(int signo);
 
 switch(pid=fork()) {
 case -1:
 	perror("Fork failed");
   	exit(1);
  case  0:
   	printf("\nCHILD: This is child process!\n");
   	printf("CHILD: My PID is-- %d\n", getpid());
   	printf("CHILD: My parent PID -- %d\n", getppid());
	while(a++ < 1000) {
	  printf("a=%d\n",a);
	  while(b++ < 10000000) {
	    while(c++ < 10000000);
	  }
	}
   	exit(0);
  default:
   	printf("PARENT: This is parent process!\n");
        printf("PARENT: My PID -- %d\n", getpid());
        printf("PARENT: My child PID %d\n",pid);
	paract.sa_handler = handler;
	paract.sa_flags = SA_RESTART;
	sigaction(SIGCHLD,&paract,&oact);
	sleep(10);
	sigaction(SIGCHLD,&oact,NULL);
	//signal(SIGCHLD,
   	exit(0);
 }
 	return 0;
}

void handler(int signo) {
        int status;
	printf("This handler has caught signal #%d\n",signo);
	pid_t wp = wait(&status);
	printf("Child process #%d finished with status #%d\n",wp,status);
}
