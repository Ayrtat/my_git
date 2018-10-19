#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 64

int main(int argc, char *argv[]) {
	int fd, nr,n=0, file_size=0;
	char s;

	if((fd=open(argv[1],O_RDONLY))<0) {
		perror("Opening failed");
		return -1;
	}
	
	lseek(fd,-1,SEEK_END);
	if(read(fd,&s,sizeof(s))<0) { 
			perror("Reading failed");
			return -1;
	}

	while((n=lseek(fd,-2,SEEK_CUR)) >= 0) {	
		if(read(fd,&s,sizeof(s))<0) { 
			perror("Reading failed");
			return -1;
		}
		printf("%c",s);
		
	}
	close(fd);
	return 0;
}
