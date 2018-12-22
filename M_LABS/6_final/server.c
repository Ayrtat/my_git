#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <syslog.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>

#define BUFFSIZE 10
#define TRUNC   syslog(LOG_INFO, "Server has recieved data, but they have been truncated");
#define SUCCESS syslog(LOG_INFO, "Server has recieved some data");

int num_store = 1;


char server_pid[12] = "/tmp/server-";

void handler(int sig) {
	chdir("/");
	char rm_temp[200] = "rm -r ";
	strcat(rm_temp, server_pid);
	system(rm_temp);
	unlink("/tmp/server_s");
	exit(0);
}

int check_msg(char *buf) {
	if(strncmp("store", buf, 5) == 0) return 0;
	if(strncmp("get",buf, 3) == 0) return 1;
	if(strncmp("HELLO", buf, 5) == 0) return 2;
	if(strncmp("remove", buf, 6) == 0) return 3;
	if(strncmp("list",buf, 4) == 0) return 4;
	if(strncmp("logout", buf, 6) == 0) return 5;
	return 6;
}

int main(int argc, char **argv) {
	struct rlimit flim;
	char gottenpid[5];
	//char buf[BUFFSIZE];
	struct buf {
		int type;
		char message[BUFFSIZE];
	} buf;

	/*Create server daemon*/
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, handler);

	pid_t fork_result;

	fork_result = fork();

	if(fork_result == -1) perror("Server can't be created");
	if(fork_result  >  0) exit(0);

	setsid(); /*the new session leader*/
	getrlimit(RLIMIT_NOFILE, &flim);
	for(int fd = 0; fd < 3; fd++) {
		close(fd);
	}

	chdir("/");
	openlog("server", LOG_PID | LOG_CONS, LOG_USER);
	syslog(LOG_INFO, "server has started the work");
	/*-------------------------*/

	
	/*Create temporary file directory*/
	if(argv[1] == NULL) {
		sprintf(gottenpid, "%d", getpid());
		strcat(server_pid, gottenpid);
		syslog(LOG_INFO, server_pid);
	
		//chdir("/tmp");
		mkdir(server_pid, 0777);
		chdir(server_pid);
	} else  {
		memset(server_pid,'\0',12);
		strcat(server_pid, argv[1]);
		mkdir(server_pid, 0666);
	}


	/*Create socket*/

	/*A UNIX domain socket address is represented in the following
       structure*/
	struct sockaddr_un serv_addr, clnt_addr, addr;
	int sockfd, saddrlen, caddrlen, max_caddrlen, n;

	if((sockfd = socket(AF_UNIX, /**/SOCK_DGRAM/**/, 0)) < 0) {
		perror("socket creating failed");
		goto exit;
	}


	unlink("/tmp/server_s");
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, "/tmp/server_s");
	saddrlen = sizeof(serv_addr.sun_family) + 
			strlen(serv_addr.sun_path);
	
	
	if(bind(sockfd, (struct sockaddr *) &serv_addr, 
					saddrlen) < 0) {
		syslog(LOG_ERR,"bind failed");
		syslog(LOG_ERR,strerror(errno));
		goto exit;
	}

	max_caddrlen = sizeof(clnt_addr);
	while(1) {
		//caddrlen = max_caddrlen;
		memset(&buf, '\0', sizeof(buf));
		n = recvfrom(sockfd, &buf, sizeof(buf), MSG_PEEK,
			(struct sockaddr *)&clnt_addr, &caddrlen);
		if(n < 0) {
			syslog(LOG_ERR,"receive failed");
			syslog(LOG_ERR,strerror(errno));
			break;
		}
		int check_result = check_msg(buf.message);
		int is_command = 0;
		if(buf.type == 1) is_command = 1;

		if(check_result == 4 && is_command) {
			pid_t pid = fork();
			if(pid == 0) {
				recvfrom(sockfd, &buf, sizeof(buf), 0,
				(struct sockaddr *)&clnt_addr, &caddrlen);
				syslog(LOG_INFO,"client was connected, PID:");
				syslog(LOG_INFO,buf.message+5);
			
				DIR *dir = NULL;
				dir = opendir(".");
			
	    			struct dirent *ent;
				struct stat istat;

	     			while ((ent = readdir (dir)) != NULL)
	     			{
					if(ent->d_type == DT_DIR) continue;
					 stat(ent->d_name, &istat); 
					 char to_send[300];
					 memset(to_send, '\0', sizeof(to_send));
					 char d_reclen[10];
					 //sprintf(d_ino, "%d", ent->d_ino);
					 sprintf(d_reclen, "%d", ent->d_reclen);
					 strcat(to_send, "File name: ");
					 strcat(to_send, ent->d_name);
					 strcat(to_send, "\nSize: ");
					 strcat(to_send, d_reclen);
					 strcat(to_send, "\nTime of last modification:");
					 strcat(to_send, asctime(gmtime(&istat.st_mtime)));
					 strcat(to_send, "\n");

					 struct buf buf;
					 memset(&buf, '\0', sizeof(buf));

					 buf.type = 2;
					 strcpy(buf.message,to_send);
					 sendto(sockfd, &buf, sizeof(buf), 0, (struct sockaddr *)&clnt_addr, caddrlen); 
				
	     			}
				sendto(sockfd, NULL, 0, 0, (struct sockaddr *)&clnt_addr, caddrlen);
				closedir(dir);
				exit(0);

			}	
			continue;
		}
		
		if(check_result == 3 && is_command) {
		   pid_t pid = fork();
		   if(pid == 0) {
			recvfrom(sockfd, &buf, sizeof(buf), 0,
			(struct sockaddr *)&clnt_addr, &caddrlen);
			syslog(LOG_INFO,"client was connected");
			//syslog(LOG_INFO,buf+5);
			int fd = open(buf.message+6, O_RDONLY);
			if (fd < 0) {
				char *msg = "file doesn't exist";
			
				struct buf buf;
				memset(&buf, '\0', sizeof(buf));
				buf.type = 2;
				strcpy(buf.message, msg);

				sendto(sockfd, &buf, sizeof(buf), 0, (struct sockaddr *)&clnt_addr, caddrlen);
				continue;
			} else sendto(sockfd, NULL, 0, 0, (struct sockaddr *)&clnt_addr, caddrlen);

			char rm_temp[40] = "rm ";
			strcat(rm_temp, buf.message+6);
			system(rm_temp);
			exit(0);
		  }
		  continue;
		}


		if(check_result == 2 && is_command) {
			recvfrom(sockfd, &buf, sizeof(buf), 0,
			(struct sockaddr *)&clnt_addr, &caddrlen);
			syslog(LOG_INFO,"client was connected, PID:");
			syslog(LOG_INFO,buf.message+5);
			sendto(sockfd, &buf, sizeof(buf), 0, (struct sockaddr *)&clnt_addr, caddrlen);
			continue;
		}
		
		if(check_result == 0 && is_command) {
				syslog(LOG_INFO, "Some proccess has required me to store a file");
				struct sockaddr_un cur_addr;

				recvfrom(sockfd, &buf, sizeof(buf), 0,
								(struct sockaddr *)&cur_addr, &caddrlen);
				char uniq[2];
				sprintf(uniq, "%d", num_store++);
				pid_t pid = fork();
				if(pid == 0) {
					printf("%s\n", uniq);
					
					int fd = creat(uniq, 0666), m;
					if(fd < 0) {
						syslog(LOG_ERR,"server can't create file to store");
						syslog(LOG_ERR,strerror(errno));
						char MSG[40] = "Impossible to store file, try later";
	
						struct buf buf;
					 	memset(&buf, '\0', sizeof(buf));
					 	buf.type = 2;
					 	strcpy(buf.message,MSG);

						sendto(sockfd, &buf, sizeof(buf), 0, (struct sockaddr *)&clnt_addr, caddrlen);
						exit(1);
					}
					else    sendto(sockfd, NULL, 0, 0, (struct sockaddr *)&clnt_addr, caddrlen);

					
					/*Current process should lock the file*/
					struct flock lock;

					lock.l_type = F_WRLCK;
					lock.l_whence = SEEK_CUR;
					lock.l_start = 0;
					lock.l_len = 0;

					/*Case when exist file will be rewritten*/
					fcntl(fd,F_SETLKW, &lock);

					int nbytes;
					while(1) {

						/*Look at top of queue*/
						int rec_res = recvfrom(sockfd, &buf, sizeof(buf), MSG_PEEK ,//| MSG_DONTWAIT,
							(struct sockaddr *)&addr, &caddrlen);
						if(rec_res == -1) break;
						/*The message is owned by current process*/
						if(strcmp(addr.sun_path, cur_addr.sun_path) == 0) {
							memset(&buf, '\0', sizeof(buf));
							
							nbytes = recvfrom(sockfd, &buf, sizeof(buf), 0,
								(struct sockaddr *)&addr, &caddrlen);
							//printf("%d\n", nbytes);
							if(nbytes == sizeof(int)) {
								lock.l_type = F_UNLCK; 
								fcntl(fd,F_SETLK,&lock);	
								struct buf buf;
					 			memset(&buf, '\0', sizeof(buf));
								buf.type = 2;
					 			strcpy(buf.message,uniq);

								sendto(sockfd, &buf, sizeof(buf), 0, (struct sockaddr *)&clnt_addr, caddrlen);
								exit(0);
							}
						/*Writing*/
		
							write(fd, buf.message, strlen(buf.message));
						}

						else continue;
					}
					exit(0);
			      }
			      continue;
	
		}
		
		if(check_result == 1 && is_command) {
			memset(&buf, '\0', sizeof(buf));
			recvfrom(sockfd, &buf, sizeof(buf), 0,
				(struct sockaddr *)&addr, &caddrlen);
			pid_t pid = fork();
			if(pid == 0) {
					syslog(LOG_INFO, "Some proccess has required me to send a file");
					int fd = open(buf.message+3, O_RDONLY), m;
					if(fd < 0) {
						syslog(LOG_ERR,"server can't open file to send");
						syslog(LOG_ERR,strerror(errno));
						
						struct buf buf;
					 	memset(&buf, '\0', sizeof(buf));
					 	buf.type = 2;
					 	strcpy(buf.message,"file doesn't exist");


						sendto(sockfd, &buf, sizeof(buf), 0, (struct sockaddr *)&clnt_addr, caddrlen);
						exit(1);
					} else sendto(sockfd, NULL, 0, 0, (struct sockaddr *)&clnt_addr, caddrlen);

					struct flock lock;

					lock.l_type = F_RDLCK;
					lock.l_whence = SEEK_CUR;
					lock.l_start = 0;
					lock.l_len = 0;

					/*Case when exist file will be read*/
					fcntl(fd,F_SETLKW, &lock);
					int nbytes;
					while( (m = read(fd,buf.message,sizeof(buf.message))) >= 0 ) {
						int res = sendto(sockfd, &buf, sizeof(int) + m, 0, (struct sockaddr *)&clnt_addr, caddrlen);
						if(m == 0) { 
							lock.l_type = F_UNLCK; 
							fcntl(fd,F_SETLK,&lock);
							break;
						}
					} 
					exit(0); 
			 }
			continue;
		}
		
	}

	
	

	/*Remove directoty*/

exit:
	closelog();
	if(argv[1] == NULL) rmdir(server_pid);
		else rmdir(argv[1]);

	exit(0);
}

