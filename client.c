#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 3333

int main(){

	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char input[1024]= {'\0'};
	char output[1024]= {'\0'};
	bool is_waiting=false;

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");

	while(1){
		if(!is_waiting){
			printf("Client: \t");
			scanf("%s", &input[0]);
			
			send(clientSocket, input, strlen(input), 0);
		
			if(strcmp(input, ":exit") == 0){
				close(clientSocket);
				printf("[-]Disconnected from server.\n");
				exit(1);
			}
		}
		
		if(recv(clientSocket, output, 1024, 0) < 0){
			printf("[-]Error in receiving data.\n");
		}else{
			printf("Server: \t%s\n", output);
		}
		
		if(strcmp(output,"You have already completed the test. Your grade will not be changed.\n")==0 ||
		strcmp(output,"This is first attempt\n") == 0){
			is_waiting=true;
		}else{
			is_waiting=false;
		}
	}

	return 0;
}







