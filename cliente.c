#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define BUFFSIZE 128

int lerOpcoesMenu(char *msg);

int main(int argc, char *argv[]) {
	int cliente_socket;
	struct sockaddr_in end_servidor;
	char buffer[BUFFSIZE];
	char mensagem[15];
	unsigned int comprimento;
	int recebido;
	
	if (argc != 3) {
		fprintf(stderr, "Utilize: Cliente <server_ip> <port>\n");
		exit(1);
	}

	// Começo do loop
	while (lerOpcoesMenu(mensagem) != 0) {
		
		recebido = 0;

		/* Create the TCP socket */
		if ((cliente_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
			perror("Erro na criacao do socket");
			exit(-1);
		}

		/* Construct the server sockaddr_in structure */
		memset(&end_servidor, 0, sizeof(end_servidor)); /* Clear struct */
		end_servidor.sin_family = AF_INET; /* Internet/IP */
		end_servidor.sin_addr.s_addr = inet_addr(argv[1]); /* IP address */
		end_servidor.sin_port = htons(atoi(argv[2])); /* server port */

		/* Establish connection */
		if (connect(cliente_socket, (struct sockaddr *) &end_servidor, sizeof(end_servidor)) < 0) {
			perror("erro no conect");
			exit(-1);
		}

		/* Send the word to the server */
		comprimento = strlen(mensagem);
		if (send(cliente_socket, mensagem, comprimento, 0) != comprimento) {
			perror("erro no envio de dados");
			exit(-1);
		}
		/* Receive the word back from the server */
		fprintf(stdout, "Recebido:\n");
		while (recebido < comprimento) {
			int bytes = 0;
			if ((bytes = recv(cliente_socket, buffer, BUFFSIZE - 1,0)) < 1) {
				perror("erro no recebimento dos dados");
				exit(-1);
			}
			recebido += bytes;
			buffer[bytes] = '\0'; /* Assure null terminated string */
			fprintf(stdout, buffer);

		}
		
		fprintf(stdout, "\n");
		close(cliente_socket);	
		buffer[0] = '\0';
	}
	// Fim do loop

	exit(0);
}

int lerOpcoesMenu(char *msg){
	int opcao;

	printf("1 - Iniciar campeonato\n");	
	printf("2 - Consultar tabela de pontos\n");
	printf("3 - Consultar cotações da rodada\n");
	printf("4 - Realizar aposta\n");
	printf("5 - Realizar teste\n");
	printf("0 - Sair\n");

	scanf("%d", &opcao);

	if (opcao == 1)
		strcpy(msg,"iniciar");
	else if (opcao == 2)
		strcpy(msg,"tabela");	
	else if (opcao == 3)
		strcpy(msg,"rodada");
	else if (opcao == 4)
		strcpy(msg,"apostar A 14.78");
	else if (opcao == 5)
		strcpy(msg,"testar P");
	else if (opcao != 0)
		lerOpcoesMenu(msg);
		
	return opcao;
}