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

int main(int argc, char *argv[]) {
	int fd = strtol(argv[1], NULL, 10);
	//close(1);
	write(1, "Welcome!\n", sizeof("Welcome!\n"));

	char msg1[SIZE];
	while(1) {
		int nread = read(0,msg1,sizeof(msg1));
		if (nread == 0) break;
			
			/*	    CHILD PROCESS
			  For writing in  the pipe - fd1[1];
			  For reading out the pipe - fd [0];*/ 
		
		int wres = write(fd, msg1, nread);
		if(wres == -1) perror("Child write");
	}
			
	exit(0);
}
