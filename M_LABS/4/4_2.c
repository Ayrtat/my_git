#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>   

#define _GNU_SOURCE  
#define SIZE 1024


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
			close(fd[0]) ;
			dup2(fd1[1],1);
			close(fd1[1]);

			char msg1[SIZE];
			while(1) {
			int nread = read(0,msg1,sizeof(msg1));
			if (nread == 0) break;
			
			/*	    CHILD PROCESS
			  For writing in  the pipe - fd1[1];
			  For reading out the pipe - fd [0];*/ 
			//sleep(4); //If the pipe is empty, read(2) will sleep until write(2) wake him up*/
			wres = write(1, msg1, nread);
			if(wres == -1) perror("Child write");
			}
			
			exit(0);
		default:
			close(fd[0]);
			close(fd1[1]);

			/*	    PARENT PROCESS
			  For writing in  the pipe - fd [1];
			  For reading out the pipe - fd1[0];*/ 
			char msg[64];
			while(1) {
				alarm(20);
				rres = read(fd1[0], msg, sizeof(msg)); /*Sleeps if pipe is empty*/
				alarm(0);
				if(rres == 0) break;
				write(1,"PARENT:\n",8);
				write(1,msg,rres);
			}
			wait(NULL);
			
	}

	exit(0);
}

