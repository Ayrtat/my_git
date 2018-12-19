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
#include <errno.h>

#define BUFFSIZE 1024

int check_msg(char *buf) {
	if(strncmp("store", buf, 5) == 0) return 0;
	if(strncmp("get",buf, 3) == 0) return 1;
	if(strncmp("HELLO", buf, 5) == 0) return 2;
	if(strncmp("remove", buf, 6) == 0) return 3;
	if(strncmp("list",buf, 4) == 0) return 4;
	if(strncmp("logout", buf, 6) == 0) return 5;
	return 6;
}

void logout(char *current_directory) {
	chdir(current_directory);
	unlink("./my_socket");
	chdir("/");

	char rm_temp[200] = "rm -r ";
	strcat(rm_temp, current_directory);
	system(rm_temp);
}


int main() {
	struct sockaddr_un serv_addr, clnt_addr;

	printf("Hello. It's the client\nYou are suggested such options:\n");
	printf("1. Store file\n2. Get file\n3. Remove file\n4. Get list\n5. LOGOUT\n"); 

	int sockfd;
	int saddrlen, caddrlen, msglen, n;

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

	char client_name[20] = "/tmp/clnt";
	char proccess_id[5];
	sprintf(proccess_id, "%d", getpid()); 
	strcat(client_name, proccess_id);

	char current_directory[20];
	memset(current_directory, '\0', sizeof(current_directory));
	strcat(current_directory, client_name);

	chdir("/tmp");
	mkdir(client_name, 0777);	
	chdir(client_name);	

	strcat(client_name, "/my_socket");

	strcpy(clnt_addr.sun_path, client_name);
	caddrlen = sizeof(clnt_addr.sun_family) + strlen(clnt_addr.sun_path);

	printf("Socket place in%s\n", clnt_addr.sun_path);
	if (bind(sockfd, (struct sockaddr *)&clnt_addr, caddrlen) < 0) {
		perror("bind failed");
		exit(1);
	}


	struct buf {
		int type; 
		char message[BUFFSIZE];
	} echo;

	memset(&echo, '\0', sizeof(echo));
	echo.type |= 1;


	char echo_hello[10] = "HELLO";
	strcat(echo_hello, proccess_id);

	strcpy(echo.message, echo_hello);
	//echo.type = 1;
	if((sendto(sockfd, &echo, sizeof(echo), 0, (struct sockaddr *)&serv_addr, saddrlen))==-1)
		{ perror("Try again"); logout(current_directory); exit(1); }
	recvfrom(sockfd, &echo, sizeof(echo), 0,
			(struct sockaddr *)&clnt_addr, &caddrlen);
	printf("Client connected to server succesfully\n");
	char command_line[40];

	struct flock lock;
	char file[40], request[100];
	int res, fd;

	while(1) {
		memset(command_line, '\0', sizeof(command_line));
		read(0, command_line, sizeof(command_line));
		
		int check = check_msg(command_line);

		switch(check) {
			case 0:
				memset(file, '\0', sizeof(file));
				printf("Enter file name that will be stored by server\n");
				res = read(0, file, sizeof(file));
				*(file + strlen(file)-1) = '\0';
				fd = open(file ,O_RDONLY);
				if(fd < 0) {
					perror("client: open failed!");
					continue;
				} 
				
				/*char request[50];
				strcat(request, "store this_file");
				strcat(*/

				struct buf {
					int type; 
					char message[BUFFSIZE];
				} buf;
				memset(&buf, '\0', sizeof(buf));
				strcpy(buf.message, "store this_file");
				buf.type = 1;

				int s = sendto(sockfd, &buf, sizeof(buf), 0, (struct sockaddr *)&serv_addr, saddrlen); 
				if(s < 0) {
					perror("Try later");
					printf("%d\n",errno);
					continue;
				}

				buf.type = 0;
		
				memset(&buf, '\0', sizeof(buf));
				
				recvfrom(sockfd, &buf, sizeof(buf), 0,
						(struct sockaddr *)&serv_addr, &caddrlen);

				char MSG[40] = "Impossible to store file, try later";
				if(strncmp(MSG, buf.message, strlen(MSG)-1) == 0) {
					printf("Server can't store, try later...");
					continue;
				}

				lock.l_type = F_RDLCK;
				lock.l_whence = SEEK_CUR;
				lock.l_start = 0;
				lock.l_len = 0;
			
				fcntl(fd,F_SETLKW, &lock);
			
				
				memset(&buf, '\0', sizeof(buf));
				while( (n = read(fd,buf.message,sizeof(buf.message))) >= 0 ) {

					int res = sendto(sockfd, &buf, sizeof(buf.type)+n, 0, (struct sockaddr *)&serv_addr, saddrlen);

					if (n == 0) {
						//sendto(sockfd, NULL, 0, 0, (struct sockaddr *)&serv_addr, saddrlen);
						lock.l_type = F_UNLCK; 
						fcntl(fd,F_SETLK,&lock);
						recvfrom(sockfd, &buf, sizeof(buf), 0,
						(struct sockaddr *)&clnt_addr, &caddrlen);
						printf("stored file id %s\n", buf.message);
						break;
					}
				} 
				break;
		     case 1:
				printf("Enter file name that will be gotten from server\n");
				memset(file, '\0', sizeof(file));
				res = read(0, file, sizeof(file));

				memset(request, '\0', sizeof(request));
				strcat(request, "get");
				strncat(request, file, res-1);

				struct buf buf_1;

				memset(&buf_1, '\0', sizeof(buf_1));
				strcpy(buf_1.message, request);
				buf_1.type = 1;

				fd = sendto(sockfd, &buf_1, sizeof(buf_1), 0, (struct sockaddr *)&serv_addr, saddrlen); 
				if(fd < 0) {
					perror("Try later");
					continue;
				}

				buf_1.type = 0;
	
				res = recvfrom(sockfd, &buf_1, sizeof(buf_1), 0,
						(struct sockaddr *)&serv_addr, &caddrlen);

				char MSG_1[40] = "file doesn't exist";
				if(res != 0) {
				if(strncmp(MSG_1, buf_1.message, strlen(MSG)-1) == 0) {
					write(1, "doesn't exitst\n", sizeof("doesn't exist\n"));
					continue;
				}
				}


			
				fd = creat(file, 0666);
				if(fd < 0) {
					perror("client1 creat failed");
					exit(1);
				}
			
			
				lock.l_type = F_WRLCK;
				lock.l_whence = SEEK_CUR;
				lock.l_start = 0;
				lock.l_len = 0;
			
				fcntl(fd,F_SETLKW, &lock);
			
				while(1) {
						memset(&buf_1, '\0', sizeof(buf_1));
						
						n = recvfrom(sockfd, &buf_1, sizeof(buf_1), 0,
							(struct sockaddr *)&serv_addr, &caddrlen);
						if(n == sizeof(int)) {
							lock.l_type = F_UNLCK;
							fcntl(fd,F_SETLKW, &lock);
							break;
						}
					/*Выдача*/
						write(fd, buf_1.message, strlen(buf_1.message));
				}
				break;
		case 3:
				memset(file, '\0', sizeof(file));
				memset(request, '\0', sizeof(request));
				printf("Enter file name that will be removed by server\n");
				//file[10];
				res = read(0, file, sizeof(file));
				*(file + strlen(file)-1) = '\0';			
				//request[50];
				strcat(request, "remove");
				strncat(request, file, res-1);

				struct buf buf_2;
				memset(&buf_2, '\0', sizeof(buf_2));
				strcpy(buf_2.message, request);
				buf_2.type = 1;

				fd = sendto(sockfd, &buf_2, sizeof(buf_2), 0, (struct sockaddr *)&serv_addr, saddrlen); 
				if(fd < 0) {
					perror("Try later");
					continue;
				}
				buf_2.type = 0;

				recvfrom(sockfd, &buf_2, sizeof(buf), 0,
						(struct sockaddr *)&serv_addr, &caddrlen);

				char MSG_2[40] = "file doesn't exist";
				if(res != 0) {
				if(strncmp(MSG_1, buf_2.message, strlen(MSG)-1) == 0) {
					write(1, "doesn't exitst\n", sizeof("doesn't exist\n"));
					continue;
				}
				}
				break;
		case 4:
				memset(file, '\0', sizeof(file));
				memset(request, '\0', sizeof(request));
				
				strcat(request, "list");

				struct buf buf_3;
				memset(&buf_3, '\0', sizeof(buf_3));
				strcpy(buf_3.message, request);
				buf_3.type = 1;
				
				fd = sendto(sockfd, &buf_3, sizeof(buf_3), 0, (struct sockaddr *)&serv_addr, saddrlen); 
				if(fd < 0) {
					perror("Try later");
					continue;
				}
				buf_3.type = 0;


				while(1) {
					memset(&buf_3, '\0', sizeof(buf_3));
					n = recvfrom(sockfd, &buf_3, sizeof(buf_3), 0,
						(struct sockaddr *)&serv_addr, &caddrlen); 
					printf("%s",buf_3.message);
					if(n == 0) break;
					
				}
				break;
		case 5:
				logout(current_directory);
				exit(0);
		default:
				write(1, "Uknown command\n", sizeof("Uknown command\n"));
		}
	}

}

