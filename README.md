# Banking App:
### Overview
This project implements a banking system using a C-based server-client architecture. The server uses fork() to create a new process for
every client connection, ensuring independent communication between multiple clients.

The program allows clients to log in, add funds, withdraw funds, and check balances. Communication between the client and server is
secured with encrypted messages.

### Features
Forking server: Handles multiple client connections independently.
Basic banking operations:
View balance
Add funds
Withdraw funds
Login/Logout system to manage multiple users.
Graceful exit handling with :exit and :logout.

### Getting Started
Prerequisites
GCC compiler
Linux/MacOS terminal (for smooth networking operations)
Basic knowledge of socket programming in C

### Setup Instructions

#### 1. Compile the Server and Client Programs:
Use the following commands to compile the programs:

gcc -o bank_server bank_server.c
gcc -o bank_client bank_client.c

#### 2. Run the Server:
Start the server by executing:

./bank_server

#### 3. Run the Client:
Open another terminal and run the client using:

./bank_client

### Explanation of Key Components
#### 1. Server Implementation (bank_server.c)

int main() {
    int sockfd, new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    // Creating the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Binding the socket
    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(sockfd, 5); // Listening for clients

    printf("Server is listening...\n");

    while (1) {
        // Accepting a new client connection
        new_sock = accept(sockfd, (struct sockaddr*)&client_addr, &addr_size);
        if (fork() == 0) {  // Creating a child process for each client
            handle_client(new_sock);  // Client-specific operations
            close(new_sock);
            exit(0);
        }
    }
}

#### Explanation:
Socket creation: socket() initializes a TCP/IP socket.
Binding: Links the socket to a port (8080 in this case).
Listening and accepting: The server listens for client connections and accepts each one in a loop.
Forking: A child process is created for each client using fork().
In each child process, handle_client() performs client-specific operations.

#### 2. Client Implementation (bank_client.c)

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // Creating the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connecting to the server
    connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("Connected to the server.\n");

    while (1) {
        printf("Enter command: ");
        fgets(buffer, 1024, stdin);  // User input

        send(sockfd, buffer, strlen(buffer), 0);  // Sending message to server

        if (strcmp(buffer, ":exit\n") == 0) {
            printf("Exiting...\n");
            break;
        }

        recv(sockfd, buffer, 1024, 0);  // Receiving response from server
        printf("Server: %s\n", buffer);
    }

    close(sockfd);  // Closing the socket
    return 0;
}
#### Explanation:
Socket creation: The client creates a socket to connect to the server.
Connecting: The connect() function establishes a connection with the server.
Command handling:
User input is sent using send().
If the command is :exit, the client disconnects.
Responses from the server are received with recv().

#### 3. Handling Client Commands (handle_client() in bank_server.c)

void handle_client(int client_sock) {
    char buffer[1024];
    int balance = 0;

    while (1) {
        recv(client_sock, buffer, 1024, 0);  // Receiving client command

        if (strcmp(buffer, ":exit\n") == 0) {
            printf("Client disconnected.\n");
            break;
        } else if (strncmp(buffer, "1 ", 2) == 0) {  // Add funds
            int amount = atoi(buffer + 2);
            balance += amount;
            sprintf(buffer, "Funds added. New balance: %d\n", balance);
        } else if (strncmp(buffer, "2 ", 2) == 0) {  // Withdraw funds
            int amount = atoi(buffer + 2);
            if (amount > balance) {
                sprintf(buffer, "Insufficient balance.\n");
            } else {
                balance -= amount;
                sprintf(buffer, "Funds withdrawn. New balance: %d\n", balance);
            }
        } else if (strcmp(buffer, "3\n") == 0) {  // View balance
            sprintf(buffer, "Current balance: %d\n", balance);
        }

        send(client_sock, buffer, strlen(buffer), 0);  // Sending response
    }
    close(client_sock);
}
#### Explanation:
Command handling: The server processes commands based on the client input.
Add funds: If the command starts with 1, the amount is extracted and added to the balance.
Withdraw funds: If the command starts with 2, the amount is checked and deducted if sufficient balance exists.
View balance: If the command is 3, the current balance is sent to the client.
Exit handling: The loop exits if the client sends :exit.

### Instructions for Use
Login
Enter your username and password as:

pass 2100

Commands:

Add funds:
1 500
Withdraw funds:
2 300
View balance:
3

Logout: Type :logout to log out and return to the login screen.
Exit: Type :exit to close the client application.

### Testing Credentials
The following credentials can be used for testing:

Username: pass | Initial Balance: 2100
Username: word | Initial Balance: 12200

### References###
https://www.geeksforgeeks.org/how-to-convert-given-number-to-a-character-array/
https://github.com/nikhilroxtomar/Multiple-Client-Server-Program-in-C-using-fork


