#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>

void handler(int signo);

int main(void) {
	static struct sigaction paract, oact;
	pid_t pid, ch_pid;

	switch(pid=fork()) {
	  case -1:
	 	perror("Fork failed");
	   	exit(1);
	  case  0:
	   	printf("CHILD #1: My PID is-- %d\n", getpid());
		sleep(1);
	   	exit(0);
	  default:
		printf("PARENT: My PID -- %d\nI ignore Cn+C and DELETE\n", getpid());
		sigset_t mask;
		switch(pid=fork()) {
			case -1: 
				perror("Fork failed");
				exit(1);
			case 0:
				printf("CHILD #2: My PID is -- %d\n", getpid());
				sleep(1);
				exit(0);
			default:
				sigemptyset(&mask);
				sigaddset(&mask,SIGINT);

				paract.sa_handler = handler;
				paract.sa_mask = mask;
				paract.sa_flags = SA_RESTART;
				sigaction(SIGCHLD,&paract,&oact);
				sleep(10);
				sigaction(SIGCHLD,&oact,NULL);
			   	exit(0);
		}
	 }
 	return 0;
}

void handler(int signo) {
        int status;
	printf("This handler has caught signal #%d\n",signo);
	sleep(2);
	pid_t wp = wait(&status);
	printf("Child process #%d finished with status #%d\n",wp,status);
}	


