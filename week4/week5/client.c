/*UDP Echo Client*/
#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 1024

#define TRUE 1
#define FALSE 0
#define MAX_CHAR 100

int checkValidString(char *param)
{
    int checkIsValid = TRUE;
    for (int i = 0; i < strlen(param) - 1; i++)
    {
        if (param[i] < 48 ||
            (param[i] > 57 && param[i] < 65) ||
            (param[i] > 90 && param[i] < 97) ||
            param[i] > 122)
        {
            checkIsValid = FALSE;
            break;
        }
    }

    if (checkIsValid)
    {
        return TRUE;
    }
    return FALSE;
}

int main(int argc, char *argv[])
{
    // IP and PORT detail
    int serverPort;
    char serverIp[MAX_CHAR];
    int client_sock;
    char buff[BUFF_SIZE], account[BUFF_SIZE],password[BUFF_SIZE],newpassword[BUFF_SIZE];
    struct sockaddr_in server_addr;
    int bytes_sent_account, bytes_sent_password, bytes_sent, bytes_received_string, bytes_received_int, bytes_received, sin_size,n;

    // Step 0: Get data from command
    if (argc != 3)
    {
        printf("Wrong command!\n");
        exit(0);
    }
    else
    {
        serverPort = atoi(argv[2]);
        strcpy(serverIp, argv[1]);
    }

    // Step 1: Construct a UDP socket
    if ((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    { /* calls socket() */
        perror("\nError: ");
        exit(0);
    }

    // Step 2: Define the address of the server by first client
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(serverPort);
    server_addr.sin_addr.s_addr = inet_addr(serverIp);

    while (TRUE)
    {
        // Step 3: Communicate with server
        sin_size = sizeof(struct sockaddr);
        
        do{
            printf("Insert account:");
            memset(account, '\0', strlen(account)-1);        
            fgets(account, BUFF_SIZE, stdin);               
        

            printf("Insert password:");
            memset(password, '\0', strlen(password)-1);        
            fgets(password, BUFF_SIZE, stdin); 
        
            bytes_sent_account = sendto(client_sock, account, strlen(account), 0, (struct sockaddr *)&server_addr, sin_size);  
            bytes_sent_password = sendto(client_sock, password, strlen(password), 0, (struct sockaddr *)&server_addr, sin_size);
                
            if (bytes_sent_password < 0 || bytes_sent_account < 0)
            {
                perror("Error: ");
                close(client_sock);
                return 0;
            }

            bytes_received = recvfrom(client_sock, buff, sizeof(buff), 0, (struct sockaddr *)&server_addr, &sin_size);
            if(strcmp(buff,"ok") == 0){
                printf("OK.\n");
            }else if(strcmp(buff,"notok") == 0){
                printf("Not OK.\n");
            }else if(strcmp(buff,"notready") == 0){
                printf("Account not ready.\n");
            }else if(strcmp(buff,"block") == 0){
                printf("Account is blocked.\n");
            }
        }while(strcmp(buff,"ok") != 0);

        do{
            printf("Insert new password or google bye:");
            memset(newpassword, '\0', strlen(newpassword));        
            fgets(newpassword, BUFF_SIZE, stdin); 
            newpassword[strlen(newpassword)-1]='\0';

            if(strcmp(newpassword,"bye")== 0){
                printf("Goodbye %s",account);                
                bytes_sent = sendto(client_sock, "bye", 3, 0, (struct sockaddr *)&server_addr, sin_size);
                printf("%d",bytes_sent);   
                close(client_sock);
            }else{
                int isValid = checkValidString(newpassword);
                if (!isValid)
                {
                    printf("Error!\n");
                    exit(0);
                }

                bytes_sent = sendto(client_sock, newpassword, strlen(newpassword), 0, (struct sockaddr *)&server_addr, sin_size);  
                printf("%d",bytes_sent);                      
                if (bytes_sent < 0 )
                {
                    perror("Error: ");
                    close(client_sock);
                    return 0;
                }

                char buff_int[BUFF_SIZE];
                char buff_string[BUFF_SIZE];

                bytes_received_int = recvfrom(client_sock, buff_int, BUFF_SIZE-1, 0, (struct sockaddr *)&server_addr, &sin_size);                 
                bytes_received_string = recvfrom(client_sock, buff_string, BUFF_SIZE-1, 0, (struct sockaddr *)&server_addr, &sin_size);
                
                printf("%d",bytes_received_int);
                printf("%d",bytes_received_string);

                if (bytes_received_string < 0 || bytes_received_int < 0)
                {
                    perror("Error: ");
                    close(client_sock);
                    return 0;
                }
                printf("Reply from server:\n");            
                printf("%s\n", buff_int);
                printf("%s\n",buff_string);                
            }        
        }while(strcmp(newpassword,"bye") != 0);        
    }
    close(client_sock);
    return 0;
}