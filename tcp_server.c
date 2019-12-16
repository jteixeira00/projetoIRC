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
#include <sodium.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>


#define SERVER_PORT 9002
#define BUF_SIZE	256
#define CHUNK_SIZE 256

void process_client(int fd,struct sockaddr_in client_addr);
void erro(char *msg);
int numbertest(char*buffer);


static int encrypt(const char *target_file, const char *source_file, const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]){
    unsigned char  buf_in[CHUNK_SIZE];
    unsigned char  buf_out[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char  header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    crypto_secretstream_xchacha20poly1305_state st;
    FILE          *fp_t, *fp_s;
    unsigned long long out_len;
    size_t         rlen;
    int            eof;
    unsigned char  tag;

    fp_s = fopen(source_file, "rb");
    fp_t = fopen(target_file, "wb");
    crypto_secretstream_xchacha20poly1305_init_push(&st, header, key);
    fwrite(header, 1, sizeof header, fp_t);
    do {
        rlen = fread(buf_in, 1, sizeof buf_in, fp_s);
        eof = feof(fp_s);
        tag = eof ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
        crypto_secretstream_xchacha20poly1305_push(&st, buf_out, &out_len, buf_in, rlen, NULL, 0, tag);
        fwrite(buf_out, 1, (size_t) out_len, fp_t);
    } while (! eof);
    fclose(fp_t);
    fclose(fp_s);
    return 0;
}


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

int seperatings(char string[BUF_SIZE]){
  
  strtok(string, "\n");
  char *word;
  char *type;
  char *encr;
  char *fich;
  word = strtok(string," ");

  if(strcmp(word,"DOWNLOAD")==0){
    type = strtok(NULL," ");
    if(type == NULL){
      printf("COMANDO INCORRETO\n");
      return -1;
    }
    encr = strtok(NULL," ");
    if(encr == NULL){
      printf("COMANDO INCORRETO\n");
      return -1;
    }
    fich = strtok(NULL," ");
    if(fich == NULL){
      printf("COMANDO INCORRETO\n");
      return -1;
    }
    if((strcmp(type,"TCP")!=0 && strcmp(type,"UDP")!=0)||(strcmp(encr,"NOR")!=0 && strcmp(encr,"ENC")!=0)||(checkfile(fich)==-1)){
      printf("COMANDO INCORRETO\n");
      return -1;
    }
    else{
      printf("%s %s %s\n",type,encr,fich);
    }
  }

  else if(strcmp(word,"LIST")==0){
    list();
  }

  else if (strcmp(word,"QUIT")==0){
    printf("QUITED\n");
  }

  else{
    printf("COMANDO INCORRETO\n");
    return -1;
  }
  return 0; 
}



void process_client(int client_fd,struct sockaddr_in client_addr)
{
  char buffer[BUF_SIZE], output[BUF_SIZE];
  char buf_int[BUF_SIZE];
  int n=0;
  unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];
  crypto_secretstream_xchacha20poly1305_keygen(key);
  FILE *file, *file1, *file2;
  char *word;
  char *type;
  char *encr;
  char *fich;
  char buf[BUF_SIZE];
	printf("Connected %s with port: %d\n", inet_ntoa(client_addr.sin_addr),htons(SERVER_PORT));
  strcpy(buffer,"");
  while(1){
    read(client_fd, buffer, BUF_SIZE);
    if(seperatings(buffer) ==2){ //download
      break;
    }
    else if(seperatings(buffer)==3){
      exit(0);
    }
  }
  sscanf(buffer, "DOWNLOAD TCP %s %s", encr, fich);


  if(strcmp(encr, "ENC") ==0){


    file= fopen("txt.txt","rb");
    file1= fopen("./ServerFiles/copia_original", "wb");
    unsigned char char_buff;
      while(fread((void*)&char_buff,1,1,file)==1){
        fwrite((void*)&char_buff, 1, 1, file1);
      }
       file2 = fopen("./ServerFiles/encrypted", "wb");
       fclose(file);
        fclose(file1);
        fclose(file2);
        memcpy(key, "This high-level API encrypts a ", crypto_secretstream_xchacha20poly1305_KEYBYTES);
        
        if(encrypt("./ServerFiles/encrypted", "./ServerFiles/copia_original", key)!=0){
          erro("Erro a encriptar\n");
        }

        
        file2 = fopen("./ServerFiles/encrypted", "rb");
  }

  else{
    char fiche[40];
    strcpy(fiche, "./ServerFiles/");
    strcat(fiche, fich);
    file2 =fopen(fiche);

  }

    n=fread(buf, 1, BUF_SIZE, file2);
    printf("%d\n",n );
    //buf[BUF_SIZE] = '\0';
    while(n){
        
        if(n==-1){
          perror("erro leitura ficheiro");
          
        }

        printf( "%s\n", buf );
        
        if(n > 0){
          printf("Sending \n");
          write(client_fd, buf, n);
        }

        n = fread(buf, 1, BUF_SIZE,file2);
        
        }
       
    
    //write(client_fd, '\0', 0);
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



