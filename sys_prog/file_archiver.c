#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>

struct afile
{
  struct dirent *fileinfo;
  struct stat file_sb;
  char info[256];
  char dirinfo[16];
  int hal; //hierarchy level
};

int i = 0;
int k = 0;
struct afile ar[64];

void archive(char *dir)   
{   
   DIR *dp; //Ukazatel na direktoriu
   struct dirent *rf; //sruktura soderzhit index i imya faila
   struct stat statbuf;//informacia o faile

   if ((dp = opendir(dir)) == NULL) {
      fprintf(stderr, "can't open directory: %s\n",dir);
      return;
   }
   chdir(dir);
   while((rf = readdir(dp)) != NULL) {
       lstat(rf->d_name, &statbuf);
       if (S_ISDIR(statbuf.st_mode)) {
	   if (strcmp(".",rf->d_name) == 0 ||
 	       strcmp("..",rf->d_name) == 0)
	     continue; //Ignorirovanie tekutschey u rodit dir
    	     i++; //vhod v uroven nizhe
	     archive(rf->d_name);
             i--; //naverh
	}
	     else {
                int nread;
                int in;
 		ar[k].fileinfo=rf;
		ar[k].file_sb=statbuf;
                strcpy(ar[k].dirinfo,dir);
	        ar[k].hal=i;
	        in = open(rf->d_name, O_RDONLY); 
		while((nread=read(in, ar[k].info, sizeof(ar[k].info)))>0) {;}
	        k++;
                   }
   }
   chdir("..");
   closedir(dp);
}

void dearchive(char *f)
{
  struct afile buf[64];
  int lf = 0;
  int nread, ac;
  int arc1=open(f,O_RDONLY, S_IRUSR|S_IWUSR);
  int r_file=read(arc1, buf, k * sizeof(struct afile));
  printf("%i\n", r_file);
  if (r_file < 0)   
    { fprintf(stderr, "Can't unpack %s\n",f);
      return;}
  while(buf[lf].hal!=0) 
   { printf("%d\n", buf[lf++].hal);} //test
  char newfolder[16] = {'\0'};
  strcpy(newfolder,buf[lf].dirinfo);
  strncat(newfolder," ",sizeof(" "));
  int er = mkdir(newfolder,S_IRWXU| S_IRWXG | S_IROTH| S_IXOTH);
  if (er == 0) printf("Folder was created succesfully\n");
   else {printf("Can't make directory:\n%s\n",strerror(errno));}
  chdir(newfolder);
  lf = 0;
  ac = 0;
  while (ac <= k)
   { ac++;
     if (lf<buf[ac].hal)
      { er = mkdir(buf[ac].dirinfo,S_IRUSR|S_IWUSR);
        if(er == 0) continue;        else {
           printf("Fail\n");
           printf("%s\n",strerror(errno));
	     }
        chdir(buf[ac].dirinfo);
        lf = buf[ac].hal; }
      if (lf>buf[ac].hal)
         { chdir(".."); }
      int out=creat(buf[ac].fileinfo->d_name, O_WRONLY|O_CREAT|O_RDONLY);
      if (out < 0) printf("%s\n",strerror(errno));
      int w_f=write(out,buf[ac].info,sizeof(buf[ac].info));
      close(out);
     }
}
	  
int main()
{ 
  int chs;
  char dir[16];
  printf("To back up a file or folder press 1\n");
  printf("If you need unpack file press 2\n");
  scanf("%i",&chs);
  if (chs == 1) 
  { printf("Enter folder or filename: \n");
    scanf("%s",dir);
    int arc=open("Archiv",O_WRONLY|O_CREAT|O_RDONLY, S_IRUSR|S_IWUSR);
    archive(dir);
    int w_file=write(arc, ar, k * sizeof(struct afile));
    if (w_file < 0)
    {
     fprintf(stderr, "Can't write %s\n",dir);
     exit(0);
    }
    close(arc);
    printf("Enter name of archive: \n");
    scanf("%s",dir);
    dearchive(dir);
  }
  if (chs == 2)
  { printf("Enter name of archive: \n");
    scanf("%s",dir);
    dearchive(dir);
  }
  exit(0);
}

