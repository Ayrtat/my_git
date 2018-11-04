#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define SIZE 1024

int main(int argc, char *argv[]) {
	int fd,fd1,fd2, n;
	char buffer[SIZE];
	
	if( (fd=open(argv[1],O_RDONLY)) < 0) {
			perror("Opening error:");
		}
		
	if ( (fd1=open(argv[1],O_RDONLY)) < 0) {
			printf("errno: %d\n",errno);		
		}		
		
	printf("%s\n",strerror(errno));
	
	return 0;
	
}
