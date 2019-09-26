#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h> /*exit(1)*/
#include <time.h>

void parse_mode(mode_t mode) {
	for(int i = 8; i != -1; i--) {
		int access = (int)mode & (1 << i);
		access >>= i;
	
		switch(i % 3) {
			case 0:
			   if(access) 
				printf("%c", 'x');
			   else printf("%c", '-');
				break;
			case 1:
				if(access) 
				printf("%c", 'w');
			   		else printf("%c", '-');
				break;

			case 2:
				if(access) 
				printf("%c", 'r');
			  		 else printf("%c", '-');
				break;
			
		}
	}
	
}


int main(int argc, char *argv[]) {

	int flag_minus_l = 0;

	if(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'l') flag_minus_l = 1;


	struct dirent *d_entry;
	DIR *dir;

	dir = opendir(".");
	if(dir == NULL) { 
		perror("couldn't open directory file");
		exit(1);
	}

	struct stat *f_info = (struct stat *) malloc(sizeof(struct stat));

	while((d_entry = readdir(dir)) != NULL) {
		

		int lres = open(d_entry->d_name, O_RDONLY);
		
		if(lres == -1) {
			perror("Couldn't get file size, opening");
			exit(1);
		}

		int fd = fstat(lres, f_info);

		if(fd == -1) {
			perror("Couldn't get file size, lstat");
			exit(1);
		}

		printf("%d\t",(unsigned int)f_info->st_size);
		close(fd);

		if(flag_minus_l) {
			parse_mode(f_info->st_mode);
			printf("\t");
			char *s = asctime(gmtime(&f_info->st_mtime));
			s[16] = '\0';
			printf("%s\t", s+10);
			printf("%d\t",f_info->st_gid);
			printf("%d\t",f_info->st_uid);
		}




		switch(d_entry->d_type) {
			case DT_DIR:
				printf("%s/\n", d_entry->d_name);
				break;
			default:
				printf("%s\n", d_entry->d_name);

		}	

	}

	

	
	return 0;
}
