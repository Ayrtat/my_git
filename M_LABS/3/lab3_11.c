#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/mman.h>

static int *S;
static sigset_t omask2;

void wakeup(int signo) { 
}

int main(void) {
	S = mmap(NULL, sizeof *S, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	pid_t pid, ch_pid;
	static struct sigaction action, caction, alm;
	void handler(int signo), handler1(int signo);
	sigset_t mask, omask;

	switch(pid=fork()) {
	  case -1:
	 	perror("Fork failed");
	   	exit(1);
	  case  0:
		ch_pid = getppid();
		caction.sa_handler = handler1;
		alm.sa_handler = wakeup;
		alm.sa_flags = SA_RESTART;
		sigaction(SIGUSR2, &caction, NULL);
		sigaction(SIGALRM, &alm, NULL);
		alarm(2);

		sigset_t mask2;
		sigaddset(&mask2,SIGUSR2);
		sigprocmask(SIG_BLOCK,&mask2,&omask2);

		kill(getppid(),SIGUSR1);
		printf("SIGUSR1 was sent to parent\n");
		sigsuspend(&omask2);
		printf("I received SIGUSR2\n");

		*S *= *S;
		printf("%d\n",*S);
		exit(0);
	   	
	  default:
		sigaddset(&mask,SIGUSR1);
		action.sa_handler = handler;
		sigaction(SIGUSR1, &action, NULL);

		sigprocmask(SIG_BLOCK,&mask,&omask);
		
		*S = 2+3+5+7+11+13+17;
		sigsuspend(&omask); /*Которые уже ожидали доставки перед вызовом*/
		printf("S=%d\n",*S);
		sigprocmask(SIG_UNBLOCK,&mask,&omask);
		kill(ch_pid,SIGUSR2);
		wait(NULL);
		exit(0);
	}
	return 0;
}


void handler(int signo) {
	printf("suspend() called me after calculating 'S'\n");
}

void handler1(int signo) {
	printf("suspend() called me after PP sent me SIGUSR2\n");
}		
