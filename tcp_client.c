/**********************************************************************
 * CLIENTE liga ao servidor (definido em argv[1]) no porto especificado  
 * (em argv[2]), escrevendo a palavra predefinida (em argv[3]).
 * USO: >cliente <enderecoServidor>  <porto>  <Palavra>
 **********************************************************************/
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
#define BUF_SIZE 1024

void erro(char *msg);
size_t strlen(const char *s);

int main(int argc, char *argv[]) {
  char endServer[100];
  
  int fd, fd_file; 
  char buffer[BUF_SIZE];
  char buf[BUF_SIZE];
  
  struct sockaddr_in addr;
  struct hostent *hostPtr;
  
  if (argc != 3) {
    	printf("cliente <host> <port> \n");
    	exit(-1);
  }

  strcpy(endServer, argv[1]);
  if ((hostPtr = gethostbyname(endServer)) == 0)
    	erro("Nao consegui obter endereço");

  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
  addr.sin_port = htons((short) atoi(argv[2]));


  if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
	erro("socket");
  }
  if( connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0){
	erro("Connect");
	}
	//scanf("%s", buffer);
	//write(fd, buffer, strlen(buffer));

	if ((fd_file=open("ficheiroRecebido.txt",O_CREAT|O_WRONLY,0600))==-1)
        {
            perror("open fail");
            exit (3);
        }
	int n= read(fd, buf, BUF_SIZE);
	printf("oi ola\n");
	while(buf[0]!='\0'){
		printf("merda\n");
		if((write(fd_file,buf,n))==-1){
                perror("write fail");
                exit (6);
            }
		
		bzero(buf, BUF_SIZE);
		n=read(fd, buf,BUF_SIZE);
		
	}

	printf("FIM DA TRANSMISSAO\n");
  	close(fd);
  	exit(0);
}

void erro(char *msg) {
	printf("Erro: %s\n", msg);
	exit(-1);
}

