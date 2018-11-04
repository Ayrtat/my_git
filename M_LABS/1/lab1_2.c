#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 256


int main(int argc, char *argv[]) {
	int fd, n,m;
	char buffer[SIZE];
	char to_write[] = "ABCD";
	buffer[SIZE]='\0';
	
	
	if((fd=creat(argv[1],strtol(argv[2], NULL,8)))<0) {
		perror("Creating file error");	
	}
	for(int i=0; i<2500; i++) {
	if((n=write(fd,to_write,sizeof(to_write))) == -1) {
		perror("Writing failed");
		return 1;
	}
	}
	if(close(fd)) { perror("Closing failed"); return 1; }
	
	if((fd=open(argv[1],O_RDONLY))<0) {
			perror("Opening failed");
			return 1;	
	}	
	
	if((m = read(fd,buffer,sizeof(buffer))) < 0) {
		perror("Reading failed");
		return 1;
	}


	printf("File %s contains:\n", argv[1]);
	printf("%s\n",buffer);
	if(n > m) printf("It has been read less than written\n"); 
			
	if((fd=open(argv[1],O_RDWR)<0)) perror("Opening failed:");
	
	return 0;
}
