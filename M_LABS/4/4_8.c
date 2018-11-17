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


int main(int argc, char *argv[]) {
	pid_t pid;
	int open_f; long long int i=0;	
	char *cmsg = "childmessage", *pmsg="parentmessage";

	struct flock lock;

	switch(pid=fork()) {
		case -1:
			perror("fork() failed");
			exit(1);
		case 0:
			lock.l_type = F_WRLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = 0;
			lock.l_len = 0;

			open_f = open(argv[1], O_WRONLY, 0777);
			if(open_f == -1) { __ERROR;  exit(1); }

			int fcl = fcntl(open_f,F_SETLK, &lock);
		
			if(fcl == -1) perror("FCNTL failed");

			while(i++ < 10000) {
				int w = write(open_f,pmsg,sizeof(pmsg));
				if(w < 0) {
					perror("Write error");
					kill(getppid(),SIGKILL);
					exit(1);
				}

			}
			lock.l_type = F_UNLCK;
			fcl = fcntl(open_f,F_SETLK, &lock);
			if(fcl == -1) perror("FCNTL failed");
			close(open_f);
			exit(0);	
		default:
			open_f = open(argv[1], O_RDONLY);
			if(open_f == -1) { __ERROR; kill(pid,SIGKILL); exit(1); }

			char ibuf[1024];

			//sleep(1);

			/*Read returns 0, parent process exits*/
			while((i = read(open_f,ibuf,sizeof(ibuf))) > 0) {
				//sleep(1);
				if(i == -1) { 
					perror("Read error"); 
					kill(pid,SIGKILL); 
					exit(1); 
				}
				write(1, ibuf, i);
			}
			
			close(open_f);
			wait(NULL);
	}
	
	exit(0);
}
