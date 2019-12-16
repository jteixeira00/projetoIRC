#include <stdio.h>
#include <stdlib.h>
// Time function, sockets, htons... file stat
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
// File function and bzero
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#define BUF_SIZE 256
#define SERVER_PORT 9003


void process_client(int fd, struct sockaddr_in client_addr);
int fdclient;
struct sockaddr_in client_addr;
void erro(char *msg);
int main(){

	int client;
	char ip[20];
	struct sockaddr_in addr;
	int client_addr_size;
	bzero((void *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
  	addr.sin_addr.s_addr = inet_addr("127.0.0.2");
  	addr.sin_port = htons(SERVER_PORT);

  	if ( (fdclient = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		erro("na funcao socket");
  	if ( bind(fdclient,(struct sockaddr*)&addr,sizeof(addr)) < 0)
		erro("na funcao bind");	
 	if( listen(fdclient, 5) < 0)
		erro("na funcao listen");
  	client_addr_size = sizeof(client_addr);

  	while (1) {
	    //clean finished child processes, avoiding zombies
	    //must use WNOHANG or would block whenever a child process was working
	    while(waitpid(-1,NULL,WNOHANG)>0);
	    //wait for new connection
	    printf("AQUI\n");
	    client = accept(fdclient,(struct sockaddr *)&client_addr,(socklen_t *)&client_addr_size);
	    printf("%d\n", fdclient);
	    inet_ntop(AF_INET,&client_addr.sin_addr,ip,INET_ADDRSTRLEN);
	    if (client > 0) {
			
	      if (fork() == 0) {
			
	        close(fdclient);
	        printf("%d\n",fdclient );
	        process_client(client,client_addr);
	        exit(0);
	      }
	    close(client);
    }
  }
}

void process_client(int client_fd, struct sockaddr_in client_addr){
	
	char buffer[BUF_SIZE], output[BUF_SIZE];
	char buf_int[BUF_SIZE];
	int n=0;
	int fd;
	struct sockaddr_in addr;
	struct hostent *hostPtr;
	int bytesReceived=0;	
	fflush(stdin);
	fflush(stdout);

	int nread = read(client_fd, buffer, BUF_SIZE-1);
	printf("nread %d\n", nread );
	buffer[nread] = '\0';
	printf("%s\n", buffer);

	
	if ((hostPtr = gethostbyname(buffer)) == 0)
    	erro("Nao consegui obter endereço");


    
  	bzero((void *) &addr, sizeof(addr));
  	addr.sin_family = AF_INET;
  	addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
  	
  	addr.sin_port = htons(9002);

  	
  	if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		erro("socket");
  	}
  	if( connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0){
		erro("Connect");
	}

	char buf[BUF_SIZE];
	printf("Connected %s with port: %d\n", inet_ntoa(client_addr.sin_addr),htons(9002));
	strcpy(buffer,"");


	while((bytesReceived = read(fd, buf, BUF_SIZE)) > 0){

	    printf("Bytes received %d\n",bytesReceived);
	 	buf[bytesReceived] = 0;
	 	write(client_fd, buf, bytesReceived);
	 	printf("%s \n", buf);

 	}

 	if(bytesReceived < 0){

 		printf("\n Read Error \n");
 	}
	





}
void erro(char *msg) {
	printf("Erro: %s\n", msg);
	exit(-1);
}