#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

//---------------------------------------------------Meathods for parsing file data---------------------------------------------------------------------------
char* parseFile(char* filepath){
    FILE* fp;
    fp = fopen(filepath, "w");

}

//---------------------------------------------------Meathods for connecting to server------------------------------------------------------------------------
//Function that, in conjuction with establishConnection(), builds a socketaddr_in struct that will be passed into a socket to link to the user given IPv4/v6
//Returns built sockaddr_in
struct sockaddr_in buildSocket(char *args[]){
    int portNumber;
	struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(args[2]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname(args[1]); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

    return serverAddress;
}

//Meathod that takes in a sockaddr_in, which should be built by buildSocket(), and establishes tha connection to the server (Error handling included)
//Returns int fileDirectory
int establishConnection(struct sockaddr_in serverAdd){
    int toSendFD;

    // Create the socket
	toSendFD = socket(AF_INET, SOCK_STREAM, 0); 

    // if there is an error
	if (toSendFD < 0){ error("CLIENT: ERROR opening socket"); exit(1); }
	
     // Connect to server Address
	if (connect(toSendFD, (struct sockaddr*)&serverAdd, sizeof(serverAdd)) < 0)
	    error("CLIENT: ERROR connecting");

    return toSendFD;
}

void closeSocket(int sockClose){
    // Write to the server the termination line
    send(sockClose, "!!@#@", strlen("!!@#@"), 0);
    close(sockClose);
    exit(0);
}

//---------------------------------------------------Meathods for interacting with Server----------------------------------------------------------------------
//Function that loops for communication between client and server, allowing to send continuous messages until the user types !@#$
void message(char *args[]){
    while(1){
        int socketFD, charsWritten;
        struct sockaddr_in serverStruct;
        char buffer[512];    
	    
        //Setup connection to server
        serverStruct = buildSocket(args);
        socketFD = establishConnection(serverStruct);

	    // Get input message from user
	    printf("CLIENT: Enter text to send to the server, and then hit enter: ");
	    memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	    fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
	    buffer[strcspn(buffer, "\n")] = '@'; // Remove the trailing \n that fgets adds
        
        //append commandCode
        if(strcmp(buffer, "!@#$") != 0){        
            strcat(buffer, "!!x000x");
        } else {
            closeSocket(socketFD);
        }
        
	    // Send message to server
	    charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	    if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	    if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	    close(socketFD); // Close the socket
    }
}

void sendFile(char* args){
        int socketFD, charsWritten;
        struct sockaddr_in serverStruct;
        char* fileToSend;
        
        //Setup connection to server
        serverStruct = buildSocket(args);
        socketFD = establishConnection(serverStruct);

        //collect the data from file directory and stuff into "fileToSend" then send data
        
	    // Send message to server
	    charsWritten = send(socketFD, fileToSend, strlen(fileToSend), 0); // Write to the server
	    if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	    if (charsWritten < strlen(fileToSend)) printf("CLIENT: WARNING: Not all data written to socket!\n");

	    close(socketFD); // Close the socket
        
}

int main(int argc, char *argv[]){
    if ((argc < 3) || (argc > 4)){ fprintf(stderr,"USAGE: %s hostname [port] [filepath]\n", argv[0]); exit(0); } // Check usage & args
    if (argc == 3){
        message(argv);
    } else if (argc == 4){ 
        sendFile(argv);
    }
	return 0;
}
