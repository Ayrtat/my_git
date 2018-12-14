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
#include <syslog.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>

#define BUFFSIZE 1024
#define TRUNC   syslog(LOG_INFO, "Server has recieved data, but they have been truncated");
#define SUCCESS syslog(LOG_INFO, "Server has recieved some data");


char server_pid[12] = "/tmp/server-";

void handler(int sig) {
	chdir("..");
	char rm_temp[40] = "rmdir --ignore-fail-on-non-empty ";
	strcat(rm_temp, server_pid);
	system(rm_temp);
	unlink("./server_s");
	exit(0);
}

int check_msg(char *buf) {
	if(strncmp("store this_file:", buf, 16) == 0) return 0;
	if(strncmp("getme this_file:", buf, 16) == 0) return 1;
	if(strncmp("HELLO", buf, 5) == 0) return 2;
	return 3;
}

int main(int argc, char **argv) {
	struct rlimit flim;
	char gottenpid[5];
	char buf[BUFFSIZE];

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
	openlog("server", LOG_PID | LOG_CONS, LOG_DAEMON);
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


	unlink("../server_s");
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, "../server_s");
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
		caddrlen = max_caddrlen;
		memset(buf, '\0', sizeof(buf));
		n = recvfrom(sockfd, buf, BUFFSIZE, MSG_PEEK,
			(struct sockaddr *)&clnt_addr, &caddrlen);
		if(n < 0) {
			syslog(LOG_ERR,"receive failed");
			syslog(LOG_ERR,strerror(errno));
			break;
		}
		int check_result = check_msg(buf);
		sleep(1);
		if(check_result == 2) {
			recvfrom(sockfd, buf, BUFFSIZE, 0,
			(struct sockaddr *)&clnt_addr, &caddrlen);
			syslog(LOG_INFO,"client was connected, PID:");
			syslog(LOG_INFO,buf+5);
			sendto(sockfd, buf, 10, 0, (struct sockaddr *)&clnt_addr, caddrlen);
			continue;
		}
		
		if(check_result == 0) {
				syslog(LOG_INFO, "Some proccess has required me to store a file");
				recvfrom(sockfd, buf, BUFFSIZE, 0,
								(struct sockaddr *)&addr, &caddrlen);
				pid_t pid = fork();
				if(pid == 0) {
					unlink(buf+6);
					int fd = creat(buf+16, 0666), m;
					if(fd < 0) {
						syslog(LOG_ERR,"server can't create file to store");
						syslog(LOG_ERR,strerror(errno));
						exit(1);
					}
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
						recvfrom(sockfd, buf, BUFFSIZE, MSG_PEEK | MSG_DONTWAIT,
							(struct sockaddr *)&addr, &caddrlen);
						/*The message is owned by current process*/
						if(strcmp(addr.sun_path, clnt_addr.sun_path) == 0) {
							memset(buf, '\0', sizeof(buf));
							alarm(4);
							nbytes = recvfrom(sockfd, buf, BUFFSIZE, 0,//MSG_DONTWAIT,
								(struct sockaddr *)&addr, &caddrlen);
							alarm(0);
							
						/*Writing*/
							write(fd, buf, nbytes);
						}

						else continue;
					}
					exit(0);
			      }
			      continue;
	
		}
		
		if(check_result == 1) {
			memset(buf, '\0', sizeof(buf));
			recvfrom(sockfd, buf, BUFFSIZE, 0,
				(struct sockaddr *)&addr, &caddrlen);
			pid_t pid = fork();
			if(pid == 0) {
					syslog(LOG_INFO, "Some proccess has required me to send a file");
					int fd = open(buf+16, 0666), m;
					if(fd < 0) {
						syslog(LOG_ERR,"server can't open file to send");
						syslog(LOG_ERR,strerror(errno));
						exit(1);
					}


					struct flock lock;

					lock.l_type = F_RDLCK;
					lock.l_whence = SEEK_CUR;
					lock.l_start = 0;
					lock.l_len = 0;

					/*Case when exist file will be read*/
					fcntl(fd,F_SETLKW, &lock);
					int nbytes;
					while( (m = read(fd,buf,sizeof(buf))) > 0 ) {
						int res = sendto(sockfd, buf, m, 0, (struct sockaddr *)&clnt_addr, caddrlen);

					} 
					exit(0); /*I hope that exit terminates all locks*/
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
