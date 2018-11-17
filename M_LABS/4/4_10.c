#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define __ERROR { perror("Opening or creating failed"); printf("File: %s; line: %d\n",__FILE__,__LINE__-1);\
		  exit(1); }

int copy(int, int, int);
char *message = "Parent:\n";
char *msg = "Child:\n";

int main(int argc, char *argv[]) {
	pid_t pid;
	int open_f; long long int i=0;	

	struct flock lock,rlock,ilock;

	switch(pid=fork()) {
		case -1:
			perror("fork() failed");
			exit(1);
		case 0:
			i = 0;
			int n1,wn1=0,rn1=0;
			char buffer1[64];
			int fcl2 =-1, fcl21 = -1;

		
			while(1) {
			      n1=read(0,buffer1,sizeof(buffer1));
			      rn1 += n1; 
			      write(1,"Ребенок говорит:\n",sizeof("Ребенок говорит:\n"));
			      wn1 += write(1,buffer1,n1);
			}
			exit(0);
		default:
			i = 0;
			char buffer[64];

			int fcl=-1, fcl1=-1;	
			int n,wn=0,rn=0;
			while(1) {
				n=read(0,buffer,sizeof(buffer));
	      			rn += n; 
				write(1,"Родитель говорит:\n",sizeof("Родитель говорит:\n"));
	      			wn += write(1,buffer,n);
			}	
			wait(NULL);
			exit(0);	
	}		
	
	exit(0);
}


