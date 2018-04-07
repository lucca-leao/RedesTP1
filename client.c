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
	//testa se recebeu o numero correto de parametros da linha de comando
		if(argc < 5){	
		fprintf(stderr , "Parametros faltando");
		exit(1);
	}
	//tamanho do nome do servidor e do nome do arquivo requisitado, ja contando com o caracter 0 no final
	size_t host_len = strlen(argv[1]) + 1;  
	size_t filename_len = strlen(argv[3]) + 1;
	
	char *nome_do_servidor = calloc(host_len, sizeof (*nome_do_servidor));
	int porta_do_servidor;
	char *nome_do_arquivo = calloc(filename_len, sizeof (*nome_do_arquivo));
	int tam_buffer = atoi(argv[4]);

	//verifica se o espaço de memória foi alocado com sucesso
	if (!nome_do_servidor) {   
		fprintf (stderr, "error: virtual memory exhausted allocating 'nome_do_servidor'\n");
		return 1;
    	}
	//salva o nome do servidor (na forma de endereço IP) recbido pela linha de comando na variável nome_do_servidor
	strncpy (nome_do_servidor, argv[1], host_len);
	
	if (!nome_do_arquivo) { 
		fprintf (stderr, "error: virtual memory exhausted allocating 'nome_do_arquivo'\n");
		return 1;
    	}
	//salva o nome do arquivo recebido pela linha de comando na variável nome_do_arquivo
	strncpy (nome_do_arquivo, argv[3], filename_len);
	
	//converte o numero de porta de string para inteiro
	porta_do_servidor = atoi(argv[2]);

	printf("Nome do servidor: %s\n", nome_do_servidor);
	printf("Porta do servidor: %d\n", porta_do_servidor);
	printf("Nome do arquivo: %s\n", nome_do_arquivo);
	printf("Tamanho do buffer: %d\n", tam_buffer);

	//cria estruturas para calcular o tempo de transferência e chama a funcao gettimeofday
	struct timeval t1;
	struct timeval t2;
	gettimeofday(&t1, NULL);
	
	// cria um socket
	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0); // o último argumento 0 estabelece uma conexão TCP
	if(network_socket < 0){
		error("Falha ao criar socket");
	}

	
	//recebe o endereço do servidor e numero de porta
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(porta_do_servidor); 
	server_address.sin_addr.s_addr = inet_addr(nome_do_servidor); 
	
	//chama a funcao connect que recebe o socket, um ponteiro que aponta para o endereço do servidor, e o tamanho do endereço
	int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));	
	if (connection_status < 0){
		error("Falha de conexao"); //connect() retorna -1 se falhar
	}
	
	//faz uma requisicao com o nome do arquivo
	send(network_socket, nome_do_arquivo, filename_len, 0);	
	printf("Sent request to server\n");
	
	//abre um arquivo para salvar os dados recebidos
	FILE *arq;
	arq = fopen(nome_do_arquivo, "w+");
	if (arq == NULL){
		printf("Problemas na criacao do arquivo");
		exit(1);	
	}
	
	int total_gravado; //recebe o numero em bytes do que foi gravado no arquivo em cada iteracao
	int total_recebido; //recebe o numero em bytes do que foi recebido pelo servidor em cada iteracao
	char server_response[tam_buffer]; //buffer que recebe os dados do servidor em cada iteracao
	int tam_arquivo = 0;
	
	do {
		//recv() salva a resposta do servidor no buffer e retorna o tamanho em bytes da resposta do servidor
		total_recebido = recv(network_socket, &server_response, tam_buffer, 0);
		tam_arquivo += total_recebido;
		//imprime a resposta do servidor
		printf("Total recebido: %d\n", total_recebido);
		int i;
		//imprime na tela os dados recebidos do servidor
		for (i=0; i<tam_buffer; i++){
			printf("Data received: %c \n", server_response[i]);
		}
		total_gravado = fwrite(server_response, 1, total_recebido, arq);
		//esvazia o buffer para a proxima iteracao do loop
		memset(server_response, 0, tam_buffer);
		if (total_gravado != total_recebido){
			printf("Erro na escrita do arquivo");
			exit(1);
		}
		
	} while(total_recebido > 0); //o processo para quando o buffer nao receber mais dados do servidor

	//encerra a conexao
	close(network_socket);
	
	//fecha o arquivo
	fclose(arq);

	//desaloca os ponteiros
	free(nome_do_servidor);
	free(nome_do_arquivo);
	
	//chama gettimeofday bara obter o tempo ao final da transferência
	gettimeofday(&t2, NULL);
	//calcula o tempo em ms e a taxa de transferencia
	double tempoGasto = (t2.tv_usec - t1.tv_usec)/1000;
	double taxa = tam_arquivo/tempoGasto;

	printf("Tamanho do arquivo: %dB\nTempo gasto:%fms\nTaxa de transferencia: %fkbps", tam_arquivo, tempoGasto, taxa);
	
	return 0;
}
