
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

/*
 *  Checks if user can log in with given credentials, saving the buffer and username params accordingly.
 *  Splits the buffer into username and passwrod, using them to open the account file and checks if given password 
 *  matches up with password from file. A defined macro gets set to the buf param gets according 
 *  to the given scenarios; Account file not found, password mismatch or succesful login.
 *  
 *  @param buf Char array buffer containing username and password separated by a space
 *  @param username Char array to store the username upon succesful login
 *  
 *  @return 0 if user could not login and 1 for succesful login
 */
int checkLogin(char *buf, char *username)
{
    FILE *infile;
    char delim[] = " ";
    char *ptr = strtok(buf, delim);
    char tempUsername[20];
    strcpy(tempUsername, ptr);
    char filename[24];
    strcpy(filename, tempUsername);
    strcat(filename, ".txt");
    ptr = strtok(NULL, delim);
    char password[10];
    strcpy(password, ptr);
    bzero(buf, sizeof(&buf));

    // Open Account file for reading
    infile = fopen (filename, "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        strcpy(buf, noUser);
        return 0;
    }
     
    // reading line by line, max 256 bytes
    const unsigned MAX_LENGTH = 256;
    char buffer[MAX_LENGTH];

    fgets(buffer, MAX_LENGTH, infile);
    buffer[strcspn(buffer, "\n")] = 0;
    if(strcmp(password, buffer) != 0 ){
        fprintf(stderr, "\nPassword %s does not match password on file %s\n", password, buffer);
        strcpy(buf, noPass);
        return 0;
    }
 
    // close file
    fclose (infile);

    strcpy(buf, login);
    strcpy(username, tempUsername);
    return 1;
}

/*
 *  Gets account data from file and stores them in passed in params.
 *  
 *  @param username Char array used to open acount file
 *  @param password Char array used to store account password for future data saving
 *  @param balance  Char array used to store balance for blance changes and future storing
 *  
 *  @return 0 if user could not access account file and 1 if data retrieval is succesful
 */
int getAccountData(char *username, char *password, int *balance)
{
    FILE *infile;
    char filename[24];
    strcpy(filename, username);
    strcat(filename, ".txt");

    // Open Account file for reading
    infile = fopen (filename, "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        return 0;
    }
     
    // reading line by line, max 256 bytes
    const unsigned MAX_LENGTH = 256;
    char buffer[MAX_LENGTH];

    fgets(buffer, MAX_LENGTH, infile);
    strcpy(password, buffer);
    password[strcspn(password, "\n")] = 0;
    fgets(buffer, MAX_LENGTH, infile);
    buffer[strcspn(buffer, "\n")] = 0;
    sscanf(buffer, "%d", balance);

    // close file
    fclose (infile);

    return 1;
}

/*
 *  Converts given integer to a int represented as a char array. 
 *  Sets array to integer '0' if nothing gets converted.
 *  
 *  @param N Int to be converted
 *  
 *  @return The converted integer char array
 */
char* convertIntegerToChar(int N)
{
    // Count digits in number N
    int m = N;
    int digit = 0;
    while (m) {
 
        // Increment number of digits
        digit++;
 
        // Truncate the last
        // digit from the number
        m /= 10;
    }
 
    // Declare char array for result
    char* arr;
 
    // Declare duplicate char array
    char arr1[digit];
 
    // Memory allocation of array
    arr = (char*)malloc(digit);
 
    // Separating integer into digits and
    // accommodate it to character array
    int index = 0;
    while (N) {
 
        // Separate last digit from
        // the number and add ASCII
        // value of character '0' is 48
        arr1[++index] = N % 10 + '0';
 
        // Truncate the last
        // digit from the number
        N /= 10;
    }
 
    // Reverse the array for result
    int i;
    for (i = 0; i < index; i++) {
        arr[i] = arr1[index - i];
    }
 
    // Char array truncate by null
    arr[i] = '\0';
 
    if(arr[0] == '\0')
        arr[0] = '0';

    // Return char array
    return (char*)arr;
}

/*
 *  Adds and saves funds stored in buffer to given param balance.
 *  Splits buffer to access added funds.
 * 
 *  @param buf Char array buffer containing added funds
 *  @param balance Int balance of users account
 */
void addFunds(char *buf, int *balance){
    int addedFunds;
    char delim[] = " ";
    char *ptr = strtok(buf, delim);
    ptr = strtok(NULL, delim);
    sscanf(ptr, "%d", &addedFunds);

    *balance = *balance + addedFunds;

    char *charBalance = convertIntegerToChar(*balance);
    strcpy(buf, charBalance);
}

/*
 *  Remove and saves funds stored in buffer from given param balance.
 *  Splits buffer to access removed funds.
 * 
 *  @param buf Char array buffer containing removed funds
 *  @param balance Int balance of users account
 */
void removeFunds(char *buf, int *balance){
    int addedFunds;
    char delim[] = " ";
    char *ptr = strtok(buf, delim);
    ptr = strtok(NULL, delim);
    sscanf(ptr, "%d", &addedFunds);

    *balance = *balance - addedFunds;
    char *charBalance = convertIntegerToChar(*balance);
    strcpy(buf, charBalance);
}

void viewBalance(char *buf, int balance){
    char *charBalance = convertIntegerToChar(balance);
    strcpy(buf, charBalance);
}

/*
 *  Saves account data to user account file. 
 *  Saves the password and balance, each on their own line in the file.
 * 
 *  @param username Char array used to open acount file
 *  @param password Char array used to store password
 *  @param balance  Char array used to store balance
 * 
 *  @return 0 if user could not access account file and 1 if data save is succesful
 */
int saveAccountData(char *username, char *password, int balance)
{
    FILE *outfile;
    char filename[24];
    strcpy(filename, username);
    strcat(filename, ".txt");

    // Open Account file for writing
    outfile = fopen (filename, "w");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        return 0;
    }
     
    char *charBalance = convertIntegerToChar(balance);
    fputs(password, outfile);
    fputs("\n", outfile);
    fputs(charBalance, outfile);

    // close file
    fclose (outfile);

    return 1;
}

int main(){
	int sockfd, ret;
	 struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[1024];
	pid_t childpid;
    FILE *fp;

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
	printf("[+]Bind to port %d\n", 8888);

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
		printf("Connection accepted from %s:%d on socket %d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port), newSocket);
        int loggedIn = 0;
        int balance = 0;
        char username[20];
        char password[10];

		if((childpid = fork()) == 0){
			close(sockfd);

			while(1){
                // Receive command from client
				recv(newSocket, buffer, 1024, 0);
                encryptData(buffer, strlen(buffer));
                printf("Client from socket %d: %s\n", newSocket, buffer);

                // Disconnet from client
                if(strcmp(buffer, ":exit") == 0){
					printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				}
                // Logout client 
                else if(strcmp(buffer, ":logout") == 0){
                    loggedIn = 0;
                    bzero(username, sizeof(username));
                }
                // Perform bank service for logged in client
                else if(loggedIn){
                    if(loggedIn){
                        getAccountData(username, password, &balance);
                        switch(buffer[0]) {
                            case '1'  :
                                addFunds(buffer, &balance);
                                saveAccountData(username, password, balance);
                                break; 
                            case '2'  :
                                removeFunds(buffer, &balance);
                                saveAccountData(username, password, balance);
                                break; 
                            case '3'  :
                                viewBalance(buffer, balance);
                            default : 
                                break;
                        }
                        printf("Buffer pre encryption: %s\n\n", buffer);
                        encryptData(buffer, strlen(buffer));
                        printf("Buffer post encryption: %s\n\n", buffer);
                        send(newSocket, buffer, strlen(buffer), 0);
                        bzero(buffer, sizeof(buffer));
                    }
                }
                // Attempt to login client
                else{
                    if (checkLogin(buffer, username))
                        loggedIn = 1;
                    encryptData(buffer, strlen(buffer));
                    send(newSocket, buffer, strlen(buffer), 0);
                    bzero(buffer, sizeof(buffer));
                }
			}
		}

	}

	close(newSocket);


	return 0;
}