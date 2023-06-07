/*UDP Echo Server*/
#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
//#define PORT 5550  /* Port that will be opened */ 
#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
    int PORT;
	int server_sock; /* file descriptors */
	char buff[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
    int firstPort = 1;
    int secondPort = 0;


    // Step 0: Get data from command
    if (argc != 2)
    {
        printf("Wrong command!./server PORT\n");
        exit(0);
    }
    else
    {
        PORT = atoi(argv[1]);
    }
	//Step 1: Construct a UDP socket
	if ((server_sock=socket(AF_INET, SOCK_DGRAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		exit(0);
	}
	
	//Step 2: Bind address to socket
	server.sin_family = AF_INET;         
	server.sin_port = htons(PORT);   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = INADDR_ANY;  /* INADDR_ANY puts your IP address automatically */   
	bzero(&(server.sin_zero),8); /* zero the rest of the structure */

  
	if(bind(server_sock,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1){ /* calls bind() */
		perror("\nError: ");
		exit(0);
	}     
	
	//Step 3: Communicate with clients
	while(1){
    sin_size = sizeof(struct sockaddr_in);
    bytes_received = recvfrom(server_sock, buff, BUFF_SIZE-1, 0, (struct sockaddr *) &client, &sin_size);
    if (bytes_received < 0) {
        perror("\nError: ");
    }
    else {
        buff[bytes_received] = '\0';
            if (firstPort != secondPort) {
                secondPort = firstPort;
                firstPort = ntohs(client.sin_port);
            }

        char numeric[BUFF_SIZE];
        char alpha[BUFF_SIZE];
        int num_idx = 0;
        int alpha_idx = 0;
        for (int i = 0; i < bytes_received; i++) {
            if (isdigit(buff[i])) {
                numeric[num_idx++] = buff[i];
            }
            else if (isalpha(buff[i])) {
                alpha[alpha_idx++] = buff[i];
            }
        }
        numeric[num_idx] = '\0';
        alpha[alpha_idx] = '\0';
        printf("[%s:%d]: numeric=%s, alpha=%s\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), numeric, alpha);
        client.sin_port = htons(secondPort);
        bytes_sent = sendto(server_sock, numeric, strlen(numeric), 0, (struct sockaddr *) &client, sin_size);
        if (bytes_sent < 0) {
            perror("\nError: ");					
        }
        bytes_sent = sendto(server_sock, alpha, strlen(alpha), 0, (struct sockaddr *) &client, sin_size);
        if (bytes_sent < 0) {
            perror("\nError: ");					
        }
    }
}

	
	close(server_sock);
	return 0;
}