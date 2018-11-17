#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>   

int main(int argc, char **argv) {
	int fd[2];

	pid_t pid[2];

	if(pipe(fd) == -1) {
		perror("PIPE FAILED");
		exit(1);
	}

	pid[0] = fork(); /*Creates new child process*/
	
	switch(pid[0]) {
		case -1:
			perror("FORK FAILED");
			exit(1);
		case  0:
		 /*duplicate fd[1] on std_out*/
			dup2(fd[1],1); close(fd[0]); close(fd[1]);
			//fflush(stdout);
			execl("/usr/bin/who","who",NULL);

			/*"who" uses std_out for output*/
	}

	pid[1] = fork(); /*Creates new child process*/

	switch(pid[1]) {
		case -1:
			perror("FORK FAILED");
			exit(1);
		case  0:
			dup2(fd[0],0); close(fd[0]); close(fd[1]);
			//fflush(stdout);
			execl("/usr/bin/wc","wc","-l",NULL);

			/*"wc" uses std_inp for input*/

	}

	close(fd[0]); close(fd[1]);

	wait(NULL);
	wait(NULL);

	exit(0);
}
