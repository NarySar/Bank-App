
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define cipherKey 'S'
#define noUser "Username not found!"
#define noPass "Password does not match!"
#define login "User has logged in!"

/*
 *  Encrypts a single char with a cipher key.
 *  
 *  @param ch Char to be encrypted
 *  
 *  @return The encrypyted char
 */
char Cipher(char ch)
{
    return ch ^ cipherKey;
}

/*
 *  Encrypts each character in an array using a Chipher function.
 *  Saves encrypted array to param buf.
 *  
 *  @param buf Char array buffer 
 *  @param s   Array size of buf
 */
void encryptData(char *buf, int s)
{
    for (int i = 0; i < s && buf[i] != '\0'; i++)
    {
        buf[i] = Cipher(buf[i]);
    }
}

int main(){
    // Setup connection
	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[1024];

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

    // Connect to server
	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");

    int loggedIn = 0;
	while(1){
        // Not logged in, attempt to login
        // :exit to exit the client
        while(!loggedIn){
            printf("Enter username and password: \t");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            encryptData(buffer, strlen(buffer));
            send(clientSocket, buffer, strlen(buffer), 0);

            encryptData(buffer, strlen(buffer));
            if(strcmp(buffer, ":exit") == 0){
                close(clientSocket);
                printf("[-]Disconnected from server.\n");
                exit(1);
            }

            bzero(buffer, sizeof(buffer));
            if(recv(clientSocket, buffer, 1024, 0) < 0){
                printf("[-]Error in receiving data.\n");
            }else{
                encryptData(buffer, strlen(buffer));
                if(strcmp(buffer, login) != 0){
                    printf("%s\n", buffer);
                }
                else{
                    loggedIn = 1;
                    printf("User has logged in\n");
                }
                
            }
        }
        // Perform bank service
        // :exit to exit the client
        // :logout to log out from account
		printf("Enter 1 and amount for adding, 2 and amount for reducing or 3 for balance: \t");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        encryptData(buffer, strlen(buffer));
		send(clientSocket, buffer, strlen(buffer), 0);

        encryptData(buffer, strlen(buffer));
        // exit
		if(strcmp(buffer, ":exit") == 0){
			close(clientSocket);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}
        // logout
		if(strcmp(buffer, ":logout") == 0){
			loggedIn = 0;
			printf("[-]Logging out from server\n");
		}
        // attempt to get bank service add/remove funds or check balance
        else{
            bzero(buffer, sizeof(buffer));
            if(recv(clientSocket, buffer, 1024, 0) < 0){
                printf("[-]Error in receiving data.\n");
            }else{
                encryptData(buffer, strlen(buffer));
                printf("Blance: \t%s\n", buffer);
            }
        }
	}

	return 0;
}