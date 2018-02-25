#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t pid, child_pid;

void main()
{
  int status;
  char *words[100];
  char proc[100];
  char sep[] = " ";
while(1) {
  printf("Please, enter the name of the process:\n");
  gets(proc);
  int mode = 1;
  int i = 0; 
  char *ptr = strtok(proc, sep);
   while (ptr != NULL)
     {
       words[i++] = ptr;
       ptr = strtok (NULL, sep);
     }
    
    if (i > 1 && strcmp(words[i-1], "x") == 0) {
	mode = 0;
	words[i-1] = NULL;
	} 
     else words[i] = NULL;
		
 if (strcmp(words[0], "break") == 0)
     exit(0);
 if (strcmp(words[0], "cd") == 0)
     { chdir(words[1]);
       continue; 
     }
  switch(pid=fork()) {
  case -1:
          perror("fork"); /* произошла ошибка */
          exit(1); /*выход из родительского процесса*/
  case 0:
          printf(" CHILD: This is child process!\n");
          printf(" CHILD: My PID is-- %d\n", getpid());
          printf(" CHILD: My parent PID -- %d\n", getppid());
          printf(" CHILD: My process was changed\n");
          (void) signal(SIGINT,SIG_DFL);
          execvp(words[0],words);
  default:
          printf("PARENT: This is parent process!\n");
          printf("PARENT: My PID -- %d\n", getpid());
          printf("PARENT: My child PID %d\n",pid);
          printf("PARENT: I'm waiting for the completion of the child process.\n");
          if (mode) {
   	  child_pid = pid;
          (void) signal(SIGINT,SIG_IGN);
          child_pid = wait(&status);
	  printf("Child has finished: PID = %d\n",child_pid);
          if (WIFEXITED(status))
 	     printf("Child exited with code %d\n",WIFEXITED(status));
          else
             printf("Child terminated abnormally\n");
          (void) signal(SIGINT,SIG_DFL);
          }
   
  }
}
}

void kill_p(int sign)
{
  kill(child_pid,sign);
}
