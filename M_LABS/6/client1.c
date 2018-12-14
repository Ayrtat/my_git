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

#define BUFFSIZE 1024

struct sockaddr_un serv_addr, clnt_addr;

void handler(int sig) {
	unlink(clnt_addr.sun_path);
	exit(0);
}

int main() {
	signal(SIGALRM, handler);
	int sockfd;
	int saddrlen, caddrlen, msglen, n;
	char buf[BUFFSIZE];

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, "/tmp/server_s");
	saddrlen = sizeof(serv_addr.sun_family) + strlen(serv_addr.sun_path);

	if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
		perror("client socket failed");
		exit(1);
	}


	bzero(&clnt_addr, sizeof(clnt_addr));
	clnt_addr.sun_family = AF_UNIX;

	char client_name[10] = "/tmp/clnt";
	char proccess_id[5];
	sprintf(proccess_id, "%d", getpid()); 
	strcat(client_name, proccess_id);
	chdir("/tmp");
	mkdir(client_name, 0777);	
	chdir(client_name);	

	strcat(client_name, "/my_socket");


	strcpy(clnt_addr.sun_path, client_name);
	printf("Socket place in%s\n", clnt_addr.sun_path);
	
	caddrlen = sizeof(clnt_addr.sun_family) + strlen(clnt_addr.sun_path);

	if (bind(sockfd, (struct sockaddr *)&clnt_addr, caddrlen) < 0) {
		perror("bind failed");
		exit(1);
	}

	char echo_hello[10] = "HELLO";
	strcat(echo_hello, proccess_id);

	sendto(sockfd, echo_hello, 10, 0, (struct sockaddr *)&serv_addr, saddrlen);
	recvfrom(sockfd, echo_hello, 10, 0,
			(struct sockaddr *)&clnt_addr, &caddrlen);

	msglen = strlen("getme this_file:M_COPY");
	
	sendto(sockfd,  "getme this_file:M_COPY", msglen, 0, (struct sockaddr *)&serv_addr, saddrlen); 


	int fd = creat("M_COPY", 0666);
	if(fd < 0) {
		perror("client1 creat failed");
		exit(1);
	}

	while(1) {
			memset(buf, '\0', sizeof(buf));
			alarm(4);
			n = recvfrom(sockfd, buf, BUFFSIZE, 0,
				(struct sockaddr *)&serv_addr, &caddrlen);
			alarm(0);
		/*Выдача*/
			write(fd, buf, n);
	}
	
}
