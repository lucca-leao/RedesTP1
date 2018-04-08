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
int main(int argc, char * argv[]){
	//testa se recebeu o numero correto de argumentos da linha de comando
	if(argc < 3){	
		fprintf(stderr , "Parametros faltando");
		exit(1);
	}
	//salva o tamanho do buffer recebido pela linha de comando
	int tam_buffer = atoi(argv[2]);
	
	
	//cria um socket
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0); // 0 é padrão para conexao TCP
	if(server_socket < 0){
		error("Falha ao criar socket");
	}
	//salva o numero da porta recebido pela linha de comando
	int portno = atoi(argv[1]);
	//define o endereco do servidor
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(portno);
	server_address.sin_addr.s_addr = inet_addr("0.0.0.0"); // INADDR_ANY; 
	
	//bind() associa um socket ao endereco do servidor
	bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	if(server_socket < 0){
		error("Falha de bind");
	}
	
	//a funcao listen() diz ao sockete que ele pode aceitar conexoes, o segundo parametro é o numero maximo de conexoes que podem ser feitas pelo socket
	listen(server_socket, 2);
	int client_socket;
	//accept() extrai a primeira conexao na fila de conexoes pendentes
	client_socket = accept(server_socket, NULL, NULL);
	
	//recebe uma requisicao do cliente contendo o nome do arquivo solicitado
	char client_request[256];
	recv(client_socket, &client_request, sizeof(client_request), 0);
	printf("Received request from client: %s\n", client_request);
	
	//abre o arquivo solicitado
	FILE *arq;
	arq = fopen(client_request, "r");
	if (arq == NULL){
		printf("Erro na abertura do arquivo");
		exit(1);
	}
	int total_lido; //guarda o tamanho do dado que sera colocado no buffer
	char buffer[tam_buffer]; //buffer para envio do arquivo
	int i;
	do {	
		total_lido = fread(buffer, 1, tam_buffer, arq);
		//imprime os dados no buffer de envio
		for(i=0; i < tam_buffer; i++){
		printf("Buffer read %i: %c\n", i, buffer[i]);}
		//envia para o socket client os dados no buffer
		send(client_socket, buffer, total_lido, 0);
		printf("Enviado: %d \n", total_lido);
		//reseta o buffer para a proxima iteracao do loop
		memset(buffer, 0, tam_buffer );
	} while(total_lido != 0); //o processo de envio para quando o buffer nao receber mais dado
	
	//encerra a conexao
	close(server_socket);
	
	//fecha o arquivo
	fclose(arq);

	return 0;
}
