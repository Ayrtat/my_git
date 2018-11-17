#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define SIZE 6144

void main(void) {
	pid_t pid;
	int fd[2],fd1[2], rres, wres, address;

	char pntmsg[] = "Message from parent\n";
	char chlmsg[] = "Message from child!\n";

	if(pipe(fd) == -1) {
		perror("PIPE FAILED");
		exit(1);
	}

	if(pipe(fd1) == -1) {
		perror("PIPE1 FAILED");
		exit(1);
	}

	pid = fork();
	/*
			     [x]fd[0] ********--------fd[1]    PARENT
			        fd[0] <-------|*******fd[1][x] CHILD
				
			        fd1[0]<-------*******fd1[1][x]PARENT
			     [x]fd1[0] *******|------fd1[1] CHILD

	*/

	switch(pid) {
		case -1:
			perror("FORK FAILED");
			exit(2);
		case  0:
			close(fd1[0]); 
			close(fd[1]) ; 
			
			/*	    CHILD PROCESS
			  For writing in  the pipe - fd1[1];
			  For reading out the pipe - fd [0];*/ 
			//sleep(1); //If the pipe is empty, read(2) will sleep until write(2) wake him up*/
			wres = write(fd1[1], chlmsg, sizeof(chlmsg));
			if(wres == -1) perror("Child write");
			char msg[SIZE];
			rres = read(fd[0], msg, sizeof(msg));
			if(rres == -1) perror("Child read");
			write(1,msg,rres);
			break;
		default:
			close(fd[0]);
			close(fd1[1]);

			/*	    PARENT PROCESS
			  For writing in  the pipe - fd [1];
			  For reading out the pipe - fd1[0];*/ 

			wres = write(fd[1], pntmsg, sizeof(chlmsg));
			if(wres == -1) perror("Parent write");
			rres = read(fd1[0], msg, sizeof(msg));
			if(rres == -1) perror("Parent read");
			write(1,msg,rres);
			wait(NULL);
	}

	exit(0);
}
