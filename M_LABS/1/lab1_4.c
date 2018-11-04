#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 64

int main(int argc, char *argv[]) {
	int fd, n;
	char buffer[SIZE];
	buffer[SIZE]='\0';
	char to_write[] = "In contrast, the British,\n today referred to in English as Anglo-Saxons, became a single nation\n bringing together Germanic peoples";
	
	if((fd=open(argv[1],O_RDWR|O_CREAT|O_TRUNC,strtol(argv[2], NULL,8)))<0) {
		perror("Opening file error");	
	}
	
	if(write(fd,to_write,sizeof(to_write))<0) {
		perror("Writing failed");
		return 1;
	}

	n = lseek(fd,0,SEEK_SET);
	if(n < 0) {
LS:		perror("LSEEK failed");
		return 2;
	}
	
   	n = read(fd,buffer,sizeof(buffer));
	if(n < 0) {
RD:		perror("Reading failed");
		return 3;
	}
	printf("SEEK_SET + 0:\n %s\n",buffer);
	
	memset(buffer,'\0',sizeof(buffer));
s
	if(lseek(fd,25,SEEK_SET) < 0) goto LS;
	if(read(fd,buffer,sizeof(buffer)) < 0) goto RD;
	printf("SEEK_SET + 25:\n %s\n",buffer);
	memset(buffer,'\0',sizeof(buffer));

	if(lseek(fd,-10,SEEK_CUR) < 0) goto LS;
	if(read(fd,buffer,sizeof(buffer))<0) goto RD;
	printf("SEEK_SET - 10:\n %s\n",buffer);
				
	if(close(fd)) perror("Closing failed");
	
	return 0;
}
