#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 64

int main(int argc, char *argv[]) {
	int err, max_size = 0;

	struct stat *f_info = malloc((argc-1)*sizeof(struct stat));
	for(int i=1; i < argc; i++) {
		err = stat(argv[1],&f_info[i]);
		if (err != 0) { 
			perror("Stat failed");
			exit(1);
		}
		if(f_info[i].st_size > max_size) max_size = f_info[i].st_size;
		
	}
	printf("Max size:%d\n", max_size);
}

