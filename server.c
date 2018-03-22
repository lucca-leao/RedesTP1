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
	if(argc < 3){	
		fprintf(stderr , "Parametros faltando");
		exit(1);
	}
	
	int tam_buffer = atoi(argv[2]);
	//char server_message[256] = "You have reached the server!";
	
	
	// Create socket
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0); // 0 is default, TCP
	if(server_socket < 0){
		error("Falha ao criar socket");
	}
	
	int portno = atoi(argv[1]);
	// Define the server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(portno);
	server_address.sin_addr.s_addr = inet_addr("0.0.0.0"); // INADDR_ANY; 
	
	// Bind 
	bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	if(server_socket < 0){
		error("Falha de bind");
	}
	
	// Listen
	listen(server_socket, 2);
	
	int client_socket; // Get client socket

	// Accept
	client_socket = accept(server_socket, NULL, NULL);
	
	// Receive request
	char client_request[256];
	recv(client_socket, &client_request, sizeof(client_request), 0);
	printf("Received request from client: %s\n", client_request);
	
	//Read file
	FILE *arq;
	arq = fopen(client_request, "r");
	if (arq == NULL){
		printf("Erro na abertura do arquivo");
		exit(1);
	}
	int total_lido; 
	char buffer[tam_buffer];
	//total_lido = fread(buffer, 1, tam_buffer, arq);
	/*while (fread(buffer, 1, tam_buffer, arq) == 5){
		printf("Buffer read 0: %c\n", buffer[0]);
		printf("Buffer read 1: %c\n", buffer[1]);
		printf("Buffer read 2: %c\n", buffer[2]);
		printf("Buffer read 3: %c\n", buffer[3]);
		printf("Buffer read 4: %c\n", buffer[4]);
		send(client_socket, buffer, tam_buffer, 0);
	}*/
	do {	
		total_lido = fread(buffer, 1, tam_buffer, arq);
		printf("Buffer read 0: %c\n", buffer[0]);
		printf("Buffer read 1: %c\n", buffer[1]);
		printf("Buffer read 2: %c\n", buffer[2]);
		printf("Buffer read 3: %c\n", buffer[3]);
		printf("Buffer read 4: %c\n", buffer[4]);
		send(client_socket, buffer, total_lido, 0);
		printf("Enviado: %d \n", total_lido);
		memset(buffer, 0, tam_buffer );
	} while(total_lido != 0);
	//send(client_socket, buffer, tam_buffer, 0);
	
	
	// Send data
	//send(client_socket, server_message, sizeof(server_message), 0);
	
	// Close socket 
	close(server_socket);
	
	// Close file
	fclose(arq);

	return 0;
}