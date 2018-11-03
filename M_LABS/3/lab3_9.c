#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>

 long long int a;

int main(void) {
 a = 0;
 long long int b=0,c=0, d=0, e = 0;
 int status;
 pid_t pid, ch_pid;
 static struct sigaction paract, oact;
 void handler(int signo);
 
 switch(pid=fork()) {
 case -1:
 	perror("Fork failed");
   	exit(1);
  case  0:
	//signal(SIGALRM, handler);
	signal(SIGALRM,SIG_DFL);
	alarm(2);
   	printf("\nCHILD: This is child process!\n");
   	printf("CHILD: My PID is-- %d\n", ch_pid=getpid());
   	printf("CHILD: My parent PID -- %d\n", getppid());
	while(a++ < 1000000) {
	  printf("a=%lld\n",a);
	  while(b++ < 10000000) {
	    while(c++ < 10000000) {
		while(d++ < 10000000) {
		  while(e++ < 10000000);
		}
            }
	  }
	}
   	exit(0);
  default:
   	printf("PARENT: This is parent process!\n");
        printf("PARENT: My PID -- %d\n", getpid());
        printf("PARENT: My child PID %d\n",pid);
	wait(NULL);
   	exit(0);
 }
 	return 0;
}

void handler(int signo) {
	printf("\nSignal was got: %d\n", signo);
	sleep(2);
	//(void) signal(SIGINT, SIG_DFL);
}
