#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define S 1024

void print_stat(struct stat info) {
	printf("Inode:%d\n",info.st_ino);
	printf("File permissions: %o\n",info.st_mode - (1 << 15));
	printf("major number:%d\n",info.st_dev >> 12);
	printf("minor number:%d\n",info.st_dev & 4095);
	printf("the number of hard links:%d\n",info.st_nlink);
	printf("file size(bytes):%d\n",info.st_size);
	printf("The user ID:%d\n",info.st_uid);
	printf("The group ID:%d\n",info.st_gid);
	printf("Last access time:\t\t\t%s",asctime(localtime(&info.st_atime)));
	printf("Last content modification time:\t\t%s",asctime(localtime(&info.st_mtime)));
	printf("Last attributes modification time:\t%s",asctime(localtime(&info.st_ctime)));
	printf("blocks:%d\n", info.st_blocks);
}
	

int main(int argc, char *argv[]) {
	int fd, n;
	char c_buf[S];

	struct stat f_info;

	if(stat(argv[1],&f_info))  { 
		perror("stat() failed");
		exit(1);
	}

	print_stat(f_info);
	return 0;
}

	

	

	
