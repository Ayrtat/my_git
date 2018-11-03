#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void hand(int sig) {
	printf("\nSignal was got: %d\n", sig);
	//(void) signal(SIGINT, SIG_DFL);
}

int main() {
	static struct sigaction new_action;
	new_action.sa_handler = hand;
	sigaction(SIGINT,&new_action,NULL);
	while(1) {
		printf("URA, 3 LABORATORNAYA!\n");
	 	sleep(1);
	}
	return 0;
}
