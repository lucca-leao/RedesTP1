#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>

void error(const char *msg){
	perror(msg);
	exit(1);
}


int main(int argc, char **argv){
		if(argc < 5){	
		fprintf(stderr , "Parametros faltando");
		exit(1);
	}

	size_t host_len = strlen(argv[1]) + 1;  //Already counting 0 terminating in string
	size_t filename_len = strlen(argv[3]) + 1;
	
	char *nome_do_servidor = calloc(host_len, sizeof (*nome_do_servidor));
	int porta_do_servidor;
	char *nome_do_arquivo = calloc(filename_len, sizeof (*nome_do_arquivo));
	int tam_buffer = atoi(argv[4]);

	//memset (nome_do_servidor, 0, host_len);
	if (!nome_do_servidor) {    /* validate memory created successfully or throw error */
		fprintf (stderr, "error: virtual memory exhausted allocating 'nome_do_servidor'\n");
		return 1;
    	}
	strncpy (nome_do_servidor, argv[1], host_len);
	
	if (!nome_do_arquivo) {    /* validate memory created successfully or throw error */
		fprintf (stderr, "error: virtual memory exhausted allocating 'nome_do_arquivo'\n");
		return 1;
    	}
	strncpy (nome_do_arquivo, argv[3], filename_len);
	
	porta_do_servidor = atoi(argv[2]);

	printf("Nome do servidor: %s\n", nome_do_servidor);
	printf("Porta do servidor: %d\n", porta_do_servidor);
	printf("Nome do arquivo: %s\n", nome_do_arquivo);
	printf("Tamanho do buffer: %d\n", tam_buffer);

	//cria estruturas para a funcao gettimeofday
	struct timeval t1;
	struct timeval t2;
	gettimeofday(&t1, NULL);
	// Create socket
	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0); // 0 is default, TCP
	if(network_socket < 0){
		error("Falha ao criar socket");
	}

	
	// Get address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(porta_do_servidor); // Port number
	server_address.sin_addr.s_addr = inet_addr(nome_do_servidor); // INADDR_ANY; 
	
	// Connet
	int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));	
	if (connection_status < 0){
		error("Falha de conexao");
	}
	
	// Send request
	send(network_socket, nome_do_arquivo, filename_len, 0);	
	printf("Sent request to server\n");
	
	// Open file
	FILE *arq;
	arq = fopen("Client_made.jpeg", "w+");
	if (arq == NULL){
		printf("Problemas na criacao do arquivo");
		exit(1);	
	}
	int total_gravado;

	// Receive
	int total_recebido;
	char server_response[tam_buffer];

	int tam_arquivo = 0;
	do {
		total_recebido = recv(network_socket, &server_response, tam_buffer, 0);
		tam_arquivo += total_recebido;
		printf("Total recebido: %d\n", total_recebido);
		// Print server response
		int i;
		for (i=0; i<5; i++){
			printf("Data received: %c \n", server_response[i]);
		}
		total_gravado = fwrite(server_response, 1, total_recebido, arq);
		memset(server_response, 0, tam_buffer);
		if (total_gravado != total_recebido){
			printf("Erro na escrita do arquivo");
			exit(1);
		}
		
	} while(total_recebido > 0);

	// Close the connection
	close(network_socket);
	
	// Close file
	fclose(arq);

	// Free pointers
	free(nome_do_servidor);
	free(nome_do_arquivo);
	gettimeofday(&t2, NULL);
	//calcula o tempo em ms
	double tempoGasto = (t2.tv_usec - t1.tv_usec)/1000;
	double taxa = tam_arquivo/tempoGasto;

	printf("Tamanho do arquivo: %dB\nTempo gasto:%fms\nTaxa de transferencia: %fkbps", tam_arquivo, tempoGasto, taxa);
	
	return 0;
}