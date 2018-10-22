#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define NRCON 5    /* Numero de conexoes */
#define BUFFSIZE 128
//#define ADDRESS "172.16.34.116"
#define ADDRESS "127.0.0.1"
#define PORTA 5001
#define NRTIMES 6
#define SALDO_INICIAL 100.00

struct Time {
	char id;
	char nome[15];
	int pontos;
	int ganhou;
	float cotacao;
} typedef Time;

struct Campeonato {
	char ip_cliente[15];
	float saldo;
	int rodada;
	Time times[NRTIMES];
} typedef Campeonato;

int num_campeonatos = 0;
Campeonato * campeonatos;

int BuscarCampeonato(char * ip_cliente) {
	for (int i = 0; i < num_campeonatos; i++) 
		if (strcmp(campeonatos[i].ip_cliente, ip_cliente) == 0)
			return i;
	return -1;
}	
	
Campeonato * AlocarCampeonato(char * ip_cliente) {
	Campeonato * c;
	int selecionado;
	
	if (num_campeonatos == 0) {
		campeonatos = (Campeonato *) malloc(sizeof(Campeonato));
		if (campeonatos == NULL) {
			perror("Erro de alocação de memória");
			exit(-1);
		}
		selecionado = num_campeonatos;
	} else {
		selecionado = BuscarCampeonato(ip_cliente);
			
		// Não encontrou cliente
		if (selecionado == -1) {
			campeonatos = realloc(campeonatos, (num_campeonatos + 1) * sizeof(Campeonato));
			if (campeonatos == NULL) {
				perror("Erro de alocação de memória");
				exit(-1);
			}
			selecionado = num_campeonatos;
		}
	}
	
	strcpy(campeonatos[selecionado].ip_cliente, ip_cliente); 
	campeonatos[selecionado].saldo = SALDO_INICIAL; 
	campeonatos[selecionado].rodada = 0; 
	
	campeonatos[selecionado].times[0].id = 'P';
	campeonatos[selecionado].times[0].pontos = 0;
	campeonatos[selecionado].times[0].ganhou = 0;
	strcpy(campeonatos[selecionado].times[0].nome, "Palmeiras");

	campeonatos[selecionado].times[1].id = 'I';
	campeonatos[selecionado].times[1].pontos = 0;
	campeonatos[selecionado].times[1].ganhou = 0;
	strcpy(campeonatos[selecionado].times[1].nome, "Internacional");
	
	campeonatos[selecionado].times[2].id = 'G';
	campeonatos[selecionado].times[2].pontos = 0;
	campeonatos[selecionado].times[2].ganhou = 0;
	strcpy(campeonatos[selecionado].times[2].nome, "Grêmio");
	
	campeonatos[selecionado].times[3].id = 'A';
	campeonatos[selecionado].times[3].pontos = 0;
	campeonatos[selecionado].times[3].ganhou = 0;
	strcpy(campeonatos[selecionado].times[3].nome, "Atlético");
	
	campeonatos[selecionado].times[4].id = 'S';
	campeonatos[selecionado].times[4].pontos = 0;
	campeonatos[selecionado].times[4].ganhou = 0;
	strcpy(campeonatos[selecionado].times[4].nome, "Santos");
	
	campeonatos[selecionado].times[5].id = 'C';
	campeonatos[selecionado].times[5].pontos = 0;
	campeonatos[selecionado].times[5].ganhou = 0;
	strcpy(campeonatos[selecionado].times[5].nome, "Corinthians");

	num_campeonatos++;

	return &campeonatos[selecionado];
}

void CalcularCotacoes(Time * a, Time * b) {
	float vitoriaA = 0, vitoriaB = 0, total = 0;
	for (int i = 0; i <= 3 + a->ganhou; i++)
		for (int j = 0; j <= 3 + b->ganhou; j++) {
			if (i > j) vitoriaA++;
			if (i < j) vitoriaB++;
			total++;
		}
	a->cotacao = total / vitoriaA;
	b->cotacao = total / vitoriaB;
}

float CalcularEmpate(float a, float b) {
	return 1 / (1 - 1/a - 1/b);
}

char * MontarTabela(char * ip_cliente) {
	Campeonato * c;
	int selecionado;
	char * s;
	char t[20];
	s = (char *) malloc(NRTIMES * 20 * sizeof(char));
	if (s == NULL) {
		perror("Erro de alocação de memória");
		exit(-1);
	}
	selecionado = BuscarCampeonato(ip_cliente);
	
	if (selecionado != -1) {
		c = &campeonatos[selecionado];
		
		s[0] = '\0';
		for (int i = 0; i < NRTIMES; i++) {
			sprintf(t, "%s %d\n", c->times[i].nome, c->times[i].pontos);
			strcat(s, t);
		}
	} else
		strcpy(s, "! Erro: cliente não iniciou o jogo.");
	
	return s;
}

char * MontarRodada(char * ip_cliente) {
	Campeonato * c;
	int selecionado, aux;
	char * s;
	char t[40];
	int p[NRTIMES];
	s = (char *) malloc(NRTIMES * 20 * sizeof(char));
	if (s == NULL) {
		perror("Erro de alocação de memória");
		exit(-1);
	}
	selecionado = BuscarCampeonato(ip_cliente);
	
	if (selecionado != -1) {
		c = &campeonatos[selecionado];

		if (c->rodada >= 2 * (NRTIMES - 1))
			strcpy(s, "! Erro: campeonato terminou.");
		else {
			s[0] = '\0';
			p[0] = 0;
			for (int i = 1; i < NRTIMES; i++)
				p[1 + ((c->rodada + i - 1) % (NRTIMES - 1))] = i;
			
			if (c->rodada >= NRTIMES - 1)
				for (int i = 0; i < NRTIMES; i = i + 2) {
					aux = p[i];
					p[i] = p[i + 1];
					p[i + 1] = aux;
				}	
			for (int i = 0; i < NRTIMES; i = i + 2) {
				CalcularCotacoes(&(c->times[p[i]]), &(c->times[p[i + 1]]));
				sprintf(t, "%c %s %.2f %c %s %.2f %c %.2f\n", 
						c->times[p[i]].id, c->times[p[i]].nome, c->times[p[i]].cotacao, 
						c->times[p[i + 1]].id, c->times[p[i + 1]].nome, c->times[p[i + 1]].cotacao, 
						'W' + i/2, CalcularEmpate(c->times[p[i]].cotacao, c->times[p[i + 1]].cotacao));
				strcat(s, t);
			}
		}
	} else
		strcpy(s, "! Erro: cliente não iniciou o jogo.");	
	
	//c->rodada++;
	
	return s;
}

void OrdenaTimes(Campeonato * c) {
	
}

void TrataCliente(int sock, struct sockaddr_in end_cliente) {
	Campeonato * c;
	char buffer[BUFFSIZE];
	char * ptr;
	char * ip_cliente = inet_ntoa(end_cliente.sin_addr);
	int recebido = -1;
	int clear = 0;

	/* Mensagem recebida */
	if ((recebido = recv(sock, buffer, BUFFSIZE - 1, 0)) < 0) {
		perror("Erro no recebimento dos dados");
		exit(-1);
	}
	buffer[recebido] = '\0';
	printf("Dados recebidos: %s\n", buffer);
	
	ptr = strtok(buffer, " ");
	
	if (strcmp(ptr, "iniciar") == 0) {
		c = AlocarCampeonato(ip_cliente);
		printf("Iniciando jogo do cliente: %s\n\n", ip_cliente);
		sprintf(ptr, "%.2f", c->saldo);
	} else if (strcmp(ptr, "tabela") == 0) {
		ptr = MontarTabela(ip_cliente);
		printf("Retornando a tabela: %s\n\n", ip_cliente);
		clear = 1;
	} else if (strcmp(ptr, "rodada") == 0) {
		ptr = MontarRodada(ip_cliente);
		printf("Retornando a rodada: %s\n\n", ip_cliente);
		clear = 1;
	} else if (strcmp(ptr, "apostar") == 0) {
		printf("Realizando aposta: %s\n\n", ip_cliente);
	} else if (strcmp(ptr, "testar") == 0) {
		printf("Testando aposta: %s\n\n", ip_cliente);
	}
		
	if (send(sock, ptr, strlen(ptr), 0) != strlen(ptr)) {
		perror("Erro no envio dos dados");
		exit(-1);
	}
	close(sock);
	if (clear) free(ptr);
}

int main(void) {
	int servidor_socket, cliente_socket;
	struct sockaddr_in end_servidor, end_cliente;

	if ((servidor_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Erro criação socket");
		exit(-1);
	}

	/* Criando a estrutura sockaddr_in do servidor */
	memset(&end_servidor, 0, sizeof(end_servidor)); /* Limpa a estrutura */
	end_servidor.sin_family = AF_INET; /* Define a família de endereço Internet/IP */
	end_servidor.sin_port = htons(PORTA); /* Porta do servidor*/
	end_servidor.sin_addr.s_addr = inet_addr(ADDRESS); /* Incoming addr */

	/* Vincula o socket a porta */
	if (bind(servidor_socket, (struct sockaddr *) &end_servidor, sizeof(end_servidor)) < 0) {
		perror("Erro no bind");
		exit(-1);
	}

	/* Coloca o socket em modo listen */
	if (listen(servidor_socket, NRCON) < 0) {
		perror("Erro no listen");
		exit(-1);
	}

	printf("Servidor rodando em %s:%d\n", inet_ntoa(end_servidor.sin_addr), PORTA);

	/* Fica em loop infinito recebendo conexões dos clientes */
	while (1) {
		unsigned int clientlen = sizeof(end_cliente);

		/* Wait for client connection */
		if ((cliente_socket = accept(servidor_socket, (struct sockaddr *) &end_cliente, &clientlen)) < 0) {
			perror("Erro accept");
			exit(-1);
		}
		fprintf(stdout, "Cliente conectado: %s\n", inet_ntoa(end_cliente.sin_addr));
		TrataCliente(cliente_socket, end_cliente);
	}

}