#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc,char *argv[]){
	int fd,outp1,outp2;

	if((fd=open(argv[1],O_RDONLY)) < 0) {
		perror("Opening failed");
		return 1;	
	}	

	int status;
	char cbuf[256], pbuf[256];
 	pid_t pid, ch_pid;
	/*ChPr and PPr have common r/w pointer*/
 
 	switch(pid=fork()) {
 	     case -1:
 		perror("Fork failed");
   		exit(1);
 	     case  0:
		if((outp1=creat("C_OUTPUT",0666))<0) {
			perror("Creating C_OUTPUT error");
			exit(1);	
	        }
		int m;
		if((m=read(fd,cbuf,sizeof(cbuf)))>0) 
		    write(outp1,cbuf,m);
		close(outp1);
		exit(0);

  	     default:
		if((outp2=creat("P_OUTPUT",0666))<0) {
			perror("Creating P_OUTPUT error");
			exit(1);	
	        }
		int pm;
		if((pm=read(fd,pbuf,sizeof(pbuf)))>0)
		    write(outp2,pbuf,pm);
   		close(outp2);
		ch_pid = wait(&status);
 	}
	
	if((outp1=open("C_OUTPUT",O_RDONLY))<0) {
		perror("OPEN C_OUTPUT error");
		exit(1);	
	}	

	if((outp2=open("P_OUTPUT",O_RDONLY))<0) {
		perror("OPEN P_OUTPUT error");
		exit(1);	
	}
	if(read(outp2,pbuf,sizeof(pbuf))>0) printf("P_OUTPUT contains:\n%s\n",pbuf);
	if(read(outp1,pbuf,sizeof(pbuf))>0) printf("C_OUTPUT contains:\n%s\n",pbuf);
	
	
	return 0;
}
