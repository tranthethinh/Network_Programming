/*UDP Echo Client*/
#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

//#define SERV_PORT 5550
//#define SERV_IP "127.0.0.1"
#define BUFF_SIZE 1024
#define maxchar 100

int checkValidString();

int main(int argc, char *argv[]){
	int SERV_PORT;
	char SERV_IP[maxchar];
	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr;
	int bytes_sent,bytes_received, sin_size;
	// Step 0: Get data from command
    if (argc != 3)
    {
        printf("Wrong command! ./client IP PORT\n");
        exit(0);
    }
    else
    {
        SERV_PORT = atoi(argv[2]);
        strcpy(SERV_IP, argv[1]);
    }
	//Step 1: Construct a UDP socket
	if ((client_sock=socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){  /* calls socket() */
		perror("\nError: ");
		exit(0);
	}

	//Step 2: Define the address of the server
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERV_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERV_IP);
	
	//Step 3: Communicate with server
	while(1){
	printf("\nInsert string to send:");
	memset(buff,'\0',(strlen(buff)+1));
	fgets(buff, BUFF_SIZE, stdin);
	if (strcmp(buff, "\n") == 0) {
        break;
    }
	int isValid = checkValidString(buff);

        if (!isValid)
        {
            printf("Error!\n");
            continue;
        }
	sin_size = sizeof(struct sockaddr);
	
	bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *) &server_addr, sin_size);
	if(bytes_sent < 0){
		perror("Error: ");
		close(client_sock);
		return 0;
	}else{

	bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *) &server_addr, &sin_size);
	if(bytes_received < 0){
		perror("Error: ");
		close(client_sock);
		return 0;
	}
	buff[bytes_received] = '\0';
	printf("Reply from server(number): %s\n", buff);

    bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *) &server_addr, &sin_size);
	if(bytes_received < 0){
		perror("Error: ");
		close(client_sock);
		return 0;
	}
	buff[bytes_received] = '\0';
	printf("Reply from server(alphabet): %s\n", buff);
	}
	}
	close(client_sock);
	return 0;
}

int checkValidString(char *param)
{
    int checkIsValid = 1;
    for (int i = 0; i < strlen(param) - 1; i++)
    {
        if (param[i] < 48 ||
            (param[i] > 57 && param[i] < 65) ||
            (param[i] > 90 && param[i] < 97) ||
            param[i] > 122)
        {
            checkIsValid = 0;
            break;
        }
    }

    if (checkIsValid)
    {
        return 1;
    }
    return 0;
}