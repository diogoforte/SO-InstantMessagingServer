#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>

#define         SERVER_PORT  6500
#define         TRUE 1

int main()
{
    int	sock, msgsock, length, yes=1;
    int putty = 0;
    struct sockaddr_in	server;
    char  rd[256];
    char  wr[512];

    sock = socket(AF_INET, SOCK_STREAM, 0); /* criar o sockaet da familia INTERNET, do tipo TCP, protocolo IP */
    if (sock < 0){
        perror("Erro na abertura do socket");
        exit(1);
    }

    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0){
        perror("Falhou a reutilização do socket.");
        exit(-1);
    }

    bzero((char *) &server, sizeof(server)); /* colocar a estrutura dos parametros do servidor a '0' */
    server.sin_family	= AF_INET; /* define a familia do socket */
    server.sin_addr.s_addr= INADDR_ANY; /* indica que o socket fica associado ao primeiro endereco IP definida na maquina */
    server.sin_port	= htons(SERVER_PORT);  /* indica ao SO que deve usar o porto predefinido */
    if(bind(sock, (struct sockaddr*) &server, sizeof(server)) < 0){ /* associa o socket ao servidor */
        perror("Nao consegui associar o socket!");
        exit(1);
    }

    length = sizeof(server);      /* obtem os parametros atribuidos ao socket para saber o porto que foi definido pelo SO */
    if(getsockname(sock, (struct sockaddr *) &server, (socklen_t *)&length)){
        perror("Nao consegui o nome do socket!");
        exit(1);
    }

    printf("Socket tem o porto #%d\n", ntohs(server.sin_port)); /* escreve no ecra o porto atribuido */
    /* ntohs() converte uma sequencia binaria de 16 bits no short */

    listen(sock,5);               /* inicia o processo de escuta */

    msgsock= accept(sock, 0, 0);  /* aguarda um pedido de ligacao, msgsock contem o descritor do canal de comunicacao com o cliente */
    if(msgsock == -1)
        perror("accept!");
    else{

        int msgLen;

        printf("Aceitei um pedido de ligação!\n");
        /* vou enviar duas mensagens para o cliente */
        sprintf(wr, "Bem vindo ao servidor de chat!!!\r\nPara terminar a ligação escreva 'exit'\r\n");
        write(msgsock, wr, strlen(wr));

        /*
         * ciclo infinito para comunicar com o cliente
         * o valor TRUE é uma macro definida no inicio deste ficheiro de código
         */
        while(TRUE){
            bzero(rd, 256);    /* limpa o buffer */
            if(read(msgsock, rd, 256) < 0){ /* le uma mensagem enviada pelo cliente */
                perror("Erro na recepcao!");
                break;       /* termina o ciclo while */
            }

            msgLen = strlen(rd); /* conta quantos caracteres tem a mensagem */
            if(strstr(rd, "exit")) /* procura a palavra "exit" na mensagem recebida */
                break;             /* termina o ciclo while */
            if(putty || strstr(rd, "putty"))
                putty = 1;         /* se vou usar o putty, na resposta tenho que incluir o '\r' */
            if(putty && strstr(rd, "\r\n")) /* o PuTTY envia estes dois caracteres em separado */
                continue;               /* ignora o recebido */

            printf("-->[%d]: %s\n", msgLen, rd); /* escreve a mensagem recebida */
            if(putty)
                sprintf(wr, "Resposta: %s\r\n", rd);   /* prepara a resposta */
            else
                sprintf(wr, "Resposta: %s\n", rd);   /* prepara a resposta */
            write(msgsock, wr, strlen(wr));      /* envia a resposta com o numero exacto de char */
        }
        close(msgsock);             /* fecha o socket associado ao canal de comunicacao com o cliente */
    }

    close(sock);                  /* fecha o canal de escuta do servidor */
    return 0;
}
