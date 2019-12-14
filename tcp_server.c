/*******************************************************************************
 * SERVIDOR no porto 9000, à escuta de novos clientes.  Quando surjem
 * novos clientes os dados por eles enviados são lidos e descarregados no ecran.
 *******************************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>
#include <strings.h>


#define SERVER_PORT     9001
#define BUF_SIZE	256

void process_client(int fd,struct sockaddr_in client_addr);
void erro(char *msg);
int numbertest(char*buffer);

int main() {
  int fd, client;
  char ip[20];
  
  
  struct sockaddr_in addr, client_addr;
  int client_addr_size;

  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(SERVER_PORT);

  if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	erro("na funcao socket");
  if ( bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0)
	erro("na funcao bind");	
  if( listen(fd, 5) < 0)
	erro("na funcao listen");
  client_addr_size = sizeof(client_addr);
  while (1) {
    //clean finished child processes, avoiding zombies
    //must use WNOHANG or would block whenever a child process was working
    while(waitpid(-1,NULL,WNOHANG)>0);
    //wait for new connection
    client = accept(fd,(struct sockaddr *)&client_addr,(socklen_t *)&client_addr_size);
    inet_ntop(AF_INET,&client_addr.sin_addr,ip,INET_ADDRSTRLEN);
    if (client > 0) {
	
      if (fork() == 0) {
		
        close(fd);
        process_client(client,client_addr);
        exit(0);
      }
    close(client);
    }
  }
  return 0;	
}

void process_client(int client_fd,struct sockaddr_in client_addr)
{
  char buffer[BUF_SIZE], output[BUF_SIZE];
  char buf_int[BUF_SIZE];
  int n=0, n_dados=0;
  int dados = 0;
  FILE *file;
  int teste;
  char buf[BUF_SIZE];
	printf("Connected %s with port: %d\n", inet_ntoa(client_addr.sin_addr),htons(SERVER_PORT));
  strcpy(buffer,"");
 

    struct stat bufferF;
    file= fopen("original.jpg","rb");
    
    

    n=fread(buf, 1, BUF_SIZE, file);
    printf("%d\n",n );
    buf[BUF_SIZE] = '\0';
    while(n){
        printf("ora bolas\n");
        if(n==-1){
          perror("erro leitura ficheiro");
          
        }

        

        printf( "%s\n", buf );
        
        if(n > 0){
          printf("Sending \n");
          write(client_fd, buf, n);
        }

        n = fread(buf, 1, BUF_SIZE,file);
        
        }
       
    
    write(client_fd, '\0', 0);
    sleep(1);
    //read(client_fd, buf_int, BUF_SIZE-1);
    fflush(stdout);

  

	close(client_fd);
}

void erro(char *msg)
{
	printf("Erro: %s\n", msg);
	exit(-1);
}



