/*UDP Echo Server*/
#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFF_SIZE 1024
#define TRUE 1
#define FALSE 0
#define MAX_CHAR 100
#define MAX_LENGTH 50

typedef struct Account {
    char username[MAX_LENGTH]; 	
    char passwords[MAX_LENGTH];
    int status;					// 1: active, 0: block    
    struct Account* next;
} Account;

void read_account_file(Account** head) {
    FILE* file = fopen("account.txt", "r");
    if (file == NULL) {								// check exist of file txt
        printf("Cannot open file account.txt!\n");
        exit(1);
    }

    char line[MAX_LENGTH*3];
    while (fgets(line, MAX_LENGTH*3, file)) {
        Account* account = (Account*) malloc(sizeof(Account));
        sscanf(line, "%s %s %d", account->username, account->passwords, &account->status);	// copy data from file txt to a linked list account         
        account->next = *head;
        *head = account;		
    }

    fclose(file);
}

void write_account_file(Account* head) {
    FILE* file = fopen("account.txt", "w");
    if (file == NULL) {
        printf("Cannot open file account.txt!\n"); // check exist of file txt
        exit(1);
    }
	Account* account;
    for (account= head; account != NULL; account = account->next) {
        fprintf(file, "%s %s %d\n", account->username, account->passwords, account->status);			// write all data of account from linked list to file txt
        
    }

    fclose(file);
}


char *getNumber(char *param)
{
    char *numberStringToken = malloc(sizeof(strlen(param) + 1));
    int count = -1;

    for (int i = 0; i < strlen(param); i++)
    {
        if (param[i] >= 48 && param[i] <= 57)
        {
            count++;
            numberStringToken[count] = param[i];
        }
    }
    numberStringToken[count+1] = '\0';

    return numberStringToken;
}

char *getStringTokenTemp(char *param)
{
    char *stringToken = malloc(sizeof(strlen(param))+1);
    int count = -1;

    for (int i = 0; i < strlen(param); i++)
    {
        if ((param[i] < 48 || param[i] > 57) && param[i]!='\n')
        {
            count++;
            stringToken[count] = param[i];
        }
    }
    stringToken[count+1] ='\0';
    return stringToken;
}

int main(int argc, char *argv[])
{
    // PORT detail
    int serverPort;
    int server_sock; /* file descriptors */
    char buff[BUFF_SIZE], account[BUFF_SIZE],password[BUFF_SIZE],buff_received[BUFF_SIZE];
    int bytes_sent_string, bytes_sent_int, bytes_received, bytes_received_account, bytes_received_password, bytes_sent,count=0;
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client;
    int sin_size;

    // Step 0: Get data from command
    if (argc != 2)
    {
        printf("Wrong command!\n");
        exit(0);
    }
    
    serverPort = atoi(argv[1]);      

    // Step 1: Construct a UDP socket
    if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    { /* calls socket() */
        perror("\nError: ");
        exit(0);
    }

    // Step 2: Bind address to socket
    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort); /* Remember htons() from "Conversions" section? =) */
    server.sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY puts your IP address automatically */
    bzero(&(server.sin_zero), 8);        /* zero the rest of the structure */

    if (bind(server_sock, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    { /* calls bind() */
        perror("\nError: ");
        exit(0);
    }
    
    while (TRUE)
    {
        // Step 3: Communicate with clients
        sin_size = sizeof(struct sockaddr_in);

        char currentaccount[BUFF_SIZE];
        int times =0;        
        Account* head = NULL;
        read_account_file(&head);
        
        do{
            bytes_received_account= recvfrom(server_sock, account, BUFF_SIZE-1, 0, (struct sockaddr *)&client, &sin_size);
            bytes_received_password = recvfrom(server_sock, password, BUFF_SIZE-1, 0, (struct sockaddr *)&client, &sin_size);
    
            if (bytes_received_account < 0 || bytes_received_password < 0 ) 
            {
                perror("\nError: ");
            
            }else{
                account[bytes_received_account-1] ='\0';
                password[bytes_received_password-1] ='\0';  

                if(strcmp(account,currentaccount)== 0){
                    times++;
                }else{
                    strcpy(currentaccount,account);
                    times = 0;
                }                

                Account* check;
                for (check = head; check != NULL; check = check->next) {
                    if(strcmp(check->username,account) == 0){
                        if(strcmp(check->passwords,password)== 0){
                            if(check->status == 0){
                                strcpy(buff,"notready");                            
                            }else{
                                strcpy(buff,"ok");
                            }
                        }else{
                            if(times==2){
                                strcpy(buff,"block");
                                check->status=0;
                                write_account_file(head);
                            }else{
                                strcpy(buff,"notok");
                            }
                                                                                                   
                        }
                        count++;
                        break;                                           
                    }                     
                }
                if(count ==0){
                    strcpy(buff,"notok");   
                }          
            }

            bytes_sent = sendto(server_sock, buff, sizeof(buff), 0, (struct sockaddr *)&client, sin_size);       /* send to the client welcome message */
            
            if (bytes_sent < 0)
            {
                perror("\nError: ");
            }
        }while(strcmp(buff,"ok") != 0);

        do{
            bytes_received = recvfrom(server_sock, buff_received, BUFF_SIZE-1, 0, (struct sockaddr *)&client, &sin_size);
            printf("%d",bytes_received);
            printf("%s",buff_received);
            if (bytes_received < 0 )
            {
                perror("\nError: ");
                close(server_sock);
            }else{
                buff_received[bytes_received]='\0';

                if(strcmp("bye",buff_received)==0){   
                    
                }else{    
                    char *buff_int = getNumber(buff_received);
                    char *buff_string = getStringTokenTemp(buff_received);

                    Account* check;
                    for (check = head; check != NULL; check = check->next) {
                        if(strcmp(check->username,currentaccount) == 0){
                            strcpy(check->passwords,buff_received);
                            write_account_file(head);
                            break;
                        }
                    }                   
                    
                    bytes_sent_int = sendto(server_sock, buff_int, sizeof(buff_int), 0, (struct sockaddr *)&client, sin_size);       /* send to the first_client welcome message */
                    bytes_sent_string = sendto(server_sock, buff_string, sizeof(buff_string), 0, (struct sockaddr *)&client, sin_size); /* send to the first_client welcome message */

                    if (bytes_sent_int < 0 || bytes_sent_string < 0)
                    {
                        perror("\nError: ");
                    }
                } 
            }            
        }while(strcmp(buff_received,"bye") != 0);
    }
    close(server_sock);
    return 0;
}