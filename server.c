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
#include <string.h>
#include <ctype.h>

#define PORT 3333

char* trim(char* input);

bool is_completed(char buffer[1024]);
char* read_questions(int, char answ[]);
void write_results(char buffer[1024], char answers[3]);

int main(){

	char answ[] = {'\0'};
	char message[128] = {'\0'};
	int sockfd, ret;
	 struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[2048] = {'\0'};
	pid_t childpid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in binding.\n");
		exit(1);
	}
	
	printf("[+]Bind to port %d\n", 3333);

	if(listen(sockfd, 10) == 0){
		printf("[+]Listening....\n");
	}else{
		printf("[-]Error in binding.\n");
	}

	while(1){
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		if((childpid = fork()) == 0){
			close(sockfd);

			while(1){
				recv(newSocket, buffer, 1024, 0);
					char answers[3]={'\0'};
				if(is_completed(buffer)){	
				send(newSocket, "You have already completed the test. Your grade will not be changed.\n",strlen("You have already completed the test. Your grade will not be changed.\n"),0);
				
					strcat(answers, read_questions(newSocket, answ));
					
					//send(newSocket,("Correct answers: %s, Wrong answers: %s",answers[0],answers[1]),strlen(("Correct answers: %s, Wrong answers: %s",answers[0],answers[1])),0); 
				}else{
					
					send(newSocket,"This is first attempt\n",strlen("This is first attempt\n"),0);
					
					strcat(answers, read_questions(newSocket, answ));

					//send(newSocket,("Correct answers: %c, Wrong answers: %c",answers,answers),strlen(("Correct answers:  , Wrong answers:  ")),0);
					
					write_results(trim(buffer),answers);
				}
					bzero(buffer, sizeof(buffer));
			}
		}
	}

	close(newSocket);

	return 0;
}

bool is_completed(char buffer[1024]){
	int fd;
	char current_name[31] = {'\0'};
	bool first_time = true;
	char current_row[38] = {'\0'};

	fd = open("results.txt", O_RDONLY);
	if(fd != -1){
		while(read(fd, current_row, 37)==37){
			strncpy(current_name, current_row, 30);
			bool is_found = strcmp(trim(current_name), trim(buffer))==0?true:false;
			if(is_found){
				close(fd);
				return true;
			}
		}
		close(fd);
		return false;
	}
}

char* read_questions(int newSocket, char answ[]){
	int fd;
	int wrong_answers = 0;
	int right_answers = 0;
	char counter[3];
	char first_answer[2] = {'\0'};
	char second_answer[2] = {'\0'};
	char current_row[130] = {'\0'};
	char question[74] = {'\0'};
	char a_answer[20] = {'\0'};
	char b_answer[18] = {'\0'};
	char c_answer[17] = {'\0'};
	char given_answer[4]={'\0'};
	char correct_answers[10][2]={{'c'},{'c'},{'b'},{'a'},{'a'},{'a'},{'c'},{'a'}};
	int count = 0;

	fd=open("questions.txt", O_RDONLY);

	if(fd != -1){
		while(read(fd,current_row,129) == 129){
			strncpy(question,current_row,73);
			strncpy(a_answer,current_row+74,19);
			strncpy(b_answer,current_row+94,17);
			strncpy(c_answer,current_row+112,16);
		
			send(newSocket,current_row,strlen(current_row),0);
			
			sleep(1);

			recv(newSocket,given_answer,strlen(given_answer),0);
			sleep(1);
			
			if(0){//sizeof(correct_answers[count])>=2
				strncpy(first_answer,given_answer,1);
				strncpy(second_answer,given_answer+2,1);
				//if(strcmp(first_answer,correct_answer[count][0])==0  || strcmp(first_answer,correct_answer[count][1])==0){
				//	right_answers++;
				//}else{
					//wrong_answers++;
				//}
				//if(strcmp(first_answer,correct_answer[count][0])==0  || strcmp(first_answer,correct_answer[count][1])==0){
				//	right_answers++;
				//}else{
				//	wrong_answers++;
				//}
			}else{
				if(strcmp(trim(given_answer),trim(correct_answers[count]))==0){
					right_answers++;
				}else{
					wrong_answers++;
				}
			}
		//bzero(current_row, sizeof(current_row));
		//bzero(given_answer, sizeof(given_answer));
		count++;		
		}
		close(fd);
	}
	answ[0] = right_answers;
	answ[1] = wrong_answers;
	
return answ;
}

void write_results(char buffer[1024], char answers[3]){

	int fd;
	char line2[100]= {'\0'};
	char line[140] = {'\0'};

	fd=open("results.txt", O_RDWR);

	if(fd !=-1){
	while(read(fd,line2,37)==37){
	}
	strcat(line,buffer);
	strcat(line,answers);
	strcat(line,answers+1);
	write(fd,line,sizeof(line));
	
	close(fd);

	}
}

char *ltrim(char *s) 
{     
    while(isspace(*s)) s++;     
    return s; 
}  

char *rtrim(char *s) 
{     
    char* back;
    int len = strlen(s);

    if(len == 0)
        return(s); 

    back = s + len;     
    while(isspace(*--back));     
    *(back+1) = '\0';     
    return s; 
}  

char *trim(char *s) 
{     
    return rtrim(ltrim(s));  
} 
