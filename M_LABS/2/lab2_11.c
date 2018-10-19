#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[], char *envp[]) {
	 int status;
	 int i = 0;
	 pid_t pid, ch_pid;
	 char *c = argv[1];
	 argv++;

	 /*char *ls    = "ls";
	 char *als[] = {"ls","-l",NULL};*/
	 
	 switch(pid=fork()) {
	 	case -1:
		 	perror("Fork failed");
		   	exit(1);
	  	case  0:
			//sleep(1);
			execvp(c,argv);
			
	  	default:
			//system("ls -l");
			exit(0);
	   	
	 }
	 return 0;
}
   
