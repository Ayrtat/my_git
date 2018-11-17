#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#define SIZE 1024

char message[] = "man is the system's manual pager.  Each page argument given to  man  isnormally  the  name of a program, utility or function.  The manual pageassociated with each of these arguments is then found and displayed.  Asection,  if  provided, will direct man to look only in that section ofthe manual.  The default action is to search in all  of  the  available sections following a pre-defined order by default, unless overridden  by  the  SECTION  directive  in /etc/manpath.config),  and  to  show only the first page found, even if page exists in several sections";	
char warning[] = "Pipe is empty or second end is not open yet\n";

void main(int argc, char *argv[]) {
	int mkn_rs = mknod(argv[1], S_IFIFO | 0666, 0), npipe, try = 0;

	if(mkn_rs == -1) {
		perror("Named pipe failed");
		exit(1);
	}
	
	pid_t pid = fork();
	switch(pid) {
		case -1:

		case  0:
			npipe = open(argv[1],O_RDONLY | O_NDELAY);
			/*It must return desc*/

			if(npipe == -1) {
				perror("Named pipe didn't open for read");
				exit(1);
			}

			printf("\nnpipe = %d\n\n", npipe);
			
			while(try++ < 100) {
				int r_res = read(npipe,message,SIZE);
				if(r_res == -1) {
					perror("Child read:");
					exit(1);
				}

				if(r_res == 0) {
					write(1,warning,sizeof(warning));
					sleep(1);
					continue;
				}

				write(1, message, r_res);
				break;

			}

			exit(0);
		default:
			while(try++ < 100) {
				npipe = open(argv[1],O_WRONLY | O_NDELAY);
				if(npipe == -1 && errno == ENXIO) { 
					perror("READ END DOESN'T OPEN YET");
					sleep(1); 
					continue; 
				}
				break;
			}

			int w_res = write(npipe,message,sizeof(message));

			if(w_res == -1) {
				perror("Parent write:");
				kill(pid,SIGKILL);
				exit(1);
				
			}
			wait(NULL);
		}
	exit(0);
}
