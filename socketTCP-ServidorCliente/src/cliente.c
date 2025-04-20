#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<netdb.h>

/* a utilizacao deste programa requer os seguintes argumentos na linha de comandos: */
/* ./cliente <nome ou IP do servidor> <porto do servidor> */
int main(int argc, char *argv[])
{
  int	 sock;
  struct sockaddr_in	server;
  struct hostent	*hp;
  char buffer[512];

  if(argc < 2){                 /* se nao escreveu dois argumentos da erro */
    perror("utilização:\n");
    perror("\tcliente <nome ou IP do servidor> <porto do servidor>\n");
    exit(1);
  }

  sock = socket(AF_INET, SOCK_STREAM, 0); /* cria socket da familia INTERNE, do tipo TCP, protocolo IP ('0') */
  if (sock < 0){
    perror("Erro na abertura do socket\n");
    exit(1);
  }

  bzero((char *) &server, sizeof(server)); /* coloca a estrura que identifica o servidor a '0' */
  hp = gethostbyname(argv[1]);  /* obtem via servidor DNS o endereco IP do servidor passado com argumento*/
  if(hp == 0){
    printf("%s: servidor desconhecido\n", argv[1]);
    exit(2);
  }

  bcopy(hp->h_addr, &server.sin_addr, hp->h_length); /* copia a sequencia binaria de 32 bits do endereco do servidor */
  server.sin_family = AF_INET;  /* indica a familia do socket */
  server.sin_port   = htons(atoi(argv[2])); /* guarda a sequencia binaria de 16 bits referente ao porto do servidor */

  if(connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0){ /* estabelece ligacao com o servidor */
    perror("Nao consegui ligar ao servidor.");
    exit(1);
  }

  /* recebe as mensagens iniciais do servidor */
  read(sock, buffer, 512);
  printf("%s", buffer);

  int msgLen;

  for(;;){
      printf("msg para o servidor: ");
      fgets(buffer, 512, stdin);             /* le mensagem do teclado */
      msgLen = strlen(buffer)-1;		   /* calcula comprimento da mensagem menos 1 char */
      buffer[msgLen] = '\0';				  /* retira o '\n' da mensagem */
      write(sock, buffer, strlen(buffer));   /* envia mensagem para o servidor */
      if(strstr(buffer, "exit"))             /* se escrevi 'exit' termino o ciclo */
          break;

      bzero(buffer, 512);       /* limpa o buffer */
      read(sock, buffer, 512);  /* le o que o servidor enviou */
      printf("%s", buffer);     /* escreve na consola */

  }
  close(sock);                  /* fecha o socket */

  return 0;
}
