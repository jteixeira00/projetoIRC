/**********************************************************************
 * CLIENTE liga ao servidor (definido em argv[1]) no porto especificado  
 * (em argv[2]), escrevendo a palavra predefinida (em argv[3]).
 * USO: >cliente <enderecoServidor>  <porto>  <Palavra>
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sodium.h>
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
#define CHUNK_SIZE 256
void erro(char *msg);
//size_t strlen(const char *s);

static int decrypt(const char *target_file, const char *source_file,
        const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES])
{
    unsigned char  buf_in[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char  buf_out[CHUNK_SIZE];
    unsigned char  header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    crypto_secretstream_xchacha20poly1305_state st;
    FILE          *fp_t, *fp_s;
    unsigned long long out_len;
    size_t         rlen;
    int            eof;
    int            ret = -1;
    unsigned char  tag;

    fp_s = fopen(source_file, "rb");
    fp_t = fopen(target_file, "wb");
    fread(header, 1, sizeof header, fp_s);
    if (crypto_secretstream_xchacha20poly1305_init_pull(&st, header, key) != 0) {
        goto ret; /* incomplete header */
    }
    do {
        rlen = fread(buf_in, 1, sizeof buf_in, fp_s);
        eof = feof(fp_s);
        if (crypto_secretstream_xchacha20poly1305_pull(&st, buf_out, &out_len, &tag,
                                                       buf_in, rlen, NULL, 0) != 0) {
            goto ret; /* corrupted chunk */
        }
        if (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && ! eof) {
            goto ret; /* premature end (end of file reached before the end of the stream) */
        }
        fwrite(buf_out, 1, (size_t) out_len, fp_t);
    } while (! eof);

    ret = 0;
ret:
    fclose(fp_t);
    fclose(fp_s);
    return ret;
}


int main(int argc, char *argv[]) {
  char endServer[100];
  
  int fd; 
  FILE *fp;
  char buffer[BUF_SIZE];
  char buf[BUF_SIZE];
  
  struct sockaddr_in addr;
  struct hostent *hostPtr;
  int bytesReceived =0;
  if (argc != 4) {
    	printf("cliente <proxy> <server> <port> \n");
    	exit(-1);
  }

  strcpy(endServer, argv[1]);
  if ((hostPtr = gethostbyname(endServer)) == 0)
    	erro("Nao consegui obter endereço");

  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
  addr.sin_port = htons((short) atoi(argv[3]));


  if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
	erro("socket");
  }
  

  if( connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0){
	erro("Connect");
	}
	printf("%s\n", argv[2]);
	strcpy(buffer, argv[2]);

	printf("%s\n", buffer );
	write(fd, argv[2], BUF_SIZE);

	fp=fopen("ficheiroRecebido","wb");
        
     

    while((bytesReceived = read(fd, buf, BUF_SIZE)) >= 0){
	    printf("Bytes received %d\n",bytesReceived);
	 	buf[bytesReceived] = 0;
		fwrite(buf, 1,bytesReceived,fp);
	 	printf("%s \n", buf);
 	}

 	if(bytesReceived < 0){

 		printf("\n Read Error \n");
 	}
 	unsigned char key[32];
 	memcpy(key, "This high-level API encrypts a ", crypto_secretstream_xchacha20poly1305_KEYBYTES);
 	fclose(fp);
 	decrypt("desencriptado.txt", "ficheiroRecebido", key);


	printf("FIM DA TRANSMISSAO\n");
  	close(fd);
  	exit(0);
}

void erro(char *msg) {
	printf("Erro: %s\n", msg);
	exit(-1);
}

