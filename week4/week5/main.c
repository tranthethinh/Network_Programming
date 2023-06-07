#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 50

typedef struct Account {
    char username[MAX_LENGTH]; 	
    char password[MAX_LENGTH];
    int status;					// 1: active, 0: block    
    struct Account* next;
} Account;

void print_menu() {
    printf("USER MANAGEMENT PROGRAM\n");
    printf("-----------------------------------\n");
    printf("1. Register\n");
    printf("2. Sign in\n");
    printf("3. Search\n");
    printf("4. Sign out\n");
    printf("Your choice (1-4, other to quit): ");
}

void read_account_file(Account** head) {
    FILE* file = fopen("account.txt", "r");
    if (file == NULL) {								// check exist of file txt
        printf("Cannot open file account.txt!\n");
        exit(1);
    }

    char line[MAX_LENGTH*3];
    while (fgets(line, MAX_LENGTH*3, file)) {
        Account* account = (Account*) malloc(sizeof(Account));
        sscanf(line, "%s %s %d", account->username, account->password, &account->status);	// copy data from file txt to a linked list account         
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
        fprintf(file, "%s %s %d\n", account->username, account->password, account->status);			// write all data of account from linked list to file txt
        
    }

    fclose(file);
}

void register_account(Account** head) {
    char username[MAX_LENGTH], password[MAX_LENGTH];
    
    printf("Username: ");
    scanf("%s", username);    // Enter the username

    // Check if the username already exists
    Account* accounts;
    for (accounts = *head; accounts != NULL; accounts = accounts->next) {
        if (strcmp(accounts->username, username) == 0) {
            printf("Account existed!\n");
            return;				//if the username already exists return to main screen
        }
    }
	printf("Password: ");
    scanf("%s", password);		// Enter the password
    
    // Create new account
    Account* account = (Account*) malloc(sizeof(Account));
    strcpy(account->username, username);
    strcpy(account->password, password);
    account->status = 1;			// intilization status of account is 1 (active)
    account->next = *head;
    *head = account;				// add new account in the first of linklist
    
    write_account_file(*head);		// Update account file into file txt

    printf("Successful registration!\n"); // Notification if complete registation 
}

Account* sign_in(Account* head) {
	char username[MAX_LENGTH], password[MAX_LENGTH];
	printf("Username: ");
	scanf("%s", username);	 
		   
	Account* account;
    for ( account = head; account != NULL; account = account->next) {
        if (strcmp(account->username, username) == 0) {	// check if there is an account has same username 
		    if(account->status == 0){					// check if status =0, account is blocked
	    		printf("Account is blocked.\n");
	    		return NULL;
			}else{
				printf("Password: ");
		    	scanf("%s", password);
	            if (strcmp(account->password, password) == 0) {
	                printf("Hello %s.\n",account->username);
	                return account;
	            } else {	                
	                if (account->status == 3) {       // if status =3, password is wrong, account is ablock 
	                    account->status = 0;		  // update status = 0
	                    printf("Password is incorrect. Account is blocked!\n"); // Notify to user
						write_account_file(head);	  //update status to file txt
	                } else {
	                    printf("Password is incorrect.You have %d more attempts.\n", 3- account->status);
						account->status++;					// if password is wrong, increase temprotary status of account, but not write this status to file txt
	                }	                
	                return NULL;
	            }
			}
        	
        }
    }
    printf("Cannot find account!\n"); 	// notify if there is an account has same username 
    return NULL;
}

void search_account(Account* head) {
	char username[MAX_LENGTH];

	printf("Username: ");
	scanf("%s", username); // Enter the username 
	for ( Account* account = head; account != NULL; account = account->next) {  
	    if (strcmp(account->username, username) == 0) {		// find account in linked list by username 
            printf("Username: %s\n", account->username);
	    	if(account->status == 0){						// print status of account 
	    		printf("Account is blocked.\n");			// if status = 0 : block
			}else{				
				printf("Account is active.\n");				// if status = 1 : active 
			}	        
	        return;
	    }
	}

	printf("Cannot find account !\n");						// cannot find account in linked list by this username
}

void sign_out(Account** current_account, Account* head) {
	
	char username[MAX_LENGTH];	
	printf("Username: ");
	scanf("%s", username);		// Enter the username								
	
	if(strcmp((*current_account)->username, username) == 0){
		printf("Goodbye %s.\n", (*current_account)->username);			// If username is signed in, account will be signed out and goodbye 
		*current_account = NULL;
	}else {																// If username is not signed in
		for ( Account* account = head; account != NULL; account = account->next) {  
	    	if (strcmp(account->username, username) == 0) {				// Check account is in licked list or no?
				printf("Account is not sign in!\n");					// if Account in linked list, notify : account is not signed in 
				return;
			}
		}
		printf("Cannot find account!\n");								// if cannot find username in licked list
	}

}

int main() {
	Account* head = NULL;
	read_account_file(&head);	
	Account* current_account = NULL;
	int choice;
	do {
	    print_menu();					// printf the menu "USER MANAGEMENT PROGRAM"
	    scanf("%d", &choice);			// enter your choice
	
	    switch (choice) {
	        case 1:						// first choice: register
	            register_account(&head);			
	            break;
	        case 2:  					// second choice: sign in
				if (current_account == NULL){					// if there is no account which is signed in now. 
					current_account = sign_in(head);			// In the same time, there is an account  which can joined.
				} else{
					printf("There is an account which is signed in.\n");
				}     
	            
	            break;
	        
	        case 3:
	            search_account(head);							// choice 3: find status of a account by username 
	            break;
	        case 4:
	            if (current_account == NULL) {					// check if has current account or no?
	                printf("There is no account which is signed in!\n");
	            } else {
	                sign_out(&current_account,head);					
	            }
	            break;
	        default:
	            printf("Goodbye!\n");
	            break;
	    }
	} while (choice >= 1 && choice <= 4);

	// Free memory
	while (head != NULL) {
	    Account* temp = head;
	    head = head->next;
	    free(temp);
	}
	
	return 0;
}

