#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define SIZE 1
#define __ERROR { perror("Opening or creating failed"); printf("File: %s; line: %d\n",__FILE__,__LINE__-1); exit(1); }

char buffer[SIZE];

int copy(int r_fd, int w_fd);

int main(int argc, char *argv[]) {
	int r_fd=0, w_fd=1, n=0, m, err;
	
	if(argc > 1) { /*Command line obtained a few arg*/
		r_fd = open(argv[1],O_RDONLY);
		if(r_fd < 0) __ERROR;

		w_fd = open(argv[2],O_RDWR|O_CREAT|O_TRUNC,0777);
		if(w_fd < 0) __ERROR;

	} 

	if(copy(r_fd,w_fd)) printf("Copying is succesful\n");
	close(r_fd); close(w_fd);
 	return 0;
}

int copy(int r_fd, int w_fd) {
	int n,wn=0,rn=0;
	while((n=read(r_fd,buffer,sizeof(buffer))) > 0) {
	      rn += n; 
	      wn += write(w_fd,buffer,n);
	}
	if(rn == wn) 
		return 1;
	return 0;
}
