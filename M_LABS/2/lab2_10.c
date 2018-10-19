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

	 char *env[] = {"LANGUAGE=en_US",NULL};
	 
	 switch(pid=fork()) {
	 	case -1:
		 	perror("Fork failed");
		   	exit(1);
	  	case  0:
			execle("lab28","lab28","L1","L2","L3",NULL, env);
			
	  	default:
			printf("The number of passed arguments %d\n",argc);
			printf("List of arguments:\n");
	
			while(argv[i]!=NULL) printf("%s ",argv[i++]);
			i = 0;
			printf("\nEnvirnoment:\n");
			while(envp[i]!=NULL) printf("%s\n",envp[i++]);
	   	
	 }
	 return 0;
}
   
