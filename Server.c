#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

int filePermissions = 0;

int main(int argc, char* argv[])
{    
    int listenSocketFD, establishedConnectionFD, portNumber, charsRead, charsWritten;
    socklen_t sizeOfClientInfo;
    char buffer[512];
    char* toRecieve = malloc(512*30);
    char toSend;
    struct sockaddr_in serverAddress, clientAddress;
    pid_t pid;

    if (argc < 2){ fprintf(stderr, "Incorrect number of arguements. Proper call is: ./Server [portnumber]"); exit(1); }

    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); 
	portNumber = atoi(argv[1]); 
	serverAddress.sin_family = AF_INET; 
	serverAddress.sin_port = htons(portNumber); 
	serverAddress.sin_addr.s_addr = INADDR_ANY;     

    listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); 
	if (listenSocketFD < 0) error("ERROR opening socket");

	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){ error("ERROR on binding");}
	listen(listenSocketFD, 5); 
    signal(SIGCHLD, SIG_IGN);

    while(1){

	    sizeOfClientInfo = sizeof(clientAddress); 
	    establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
	    if (establishedConnectionFD < 0) error("ERROR on accept");

        if((pid = fork()) == -1){
            close(establishedConnectionFD);
            continue;

        } else if(pid > 0){
            close(establishedConnectionFD);
            continue;

        } else if(pid == 0){

	        memset(buffer, '\0', 512);
            while((strstr(buffer, "!!@#@") == NULL) && (strstr(toRecieve, "!!x000x") == NULL) && (strstr(toRecieve, "!!x001x") == NULL) && (strstr(toRecieve, "!!x002x") == NULL))
            {
                memset(buffer, '\0', 512);
	            charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer), 0); 
                strcat(toRecieve, buffer);
            }
    
	        if (charsRead < 0) error("ERROR reading from socket");


            if(strstr(toRecieve, "!!@#@") != NULL){
                printf("Client Disconnected");
                free(toRecieve);
                close(establishedConnectionFD);
                return 0;
            } else if(strstr(toRecieve, "!!x000x") != NULL){ 
                toRecieve[strlen(toRecieve) - 8] = '\0';
                printf("I the SERVER revieceved: %s\n", toRecieve);
            } else if(strstr(toRecieve, "!!x001x") != NULL){
                //ask permission and write to file
            } else if(strstr(toRecieve, "!!x002x") != NULL){
                //ask permission and write to jpg
            }
    
            memset(toRecieve, '\0', strlen(toRecieve));
    
	        close(establishedConnectionFD); 
            free(toRecieve);
            return 0;
        }
    }
	close(listenSocketFD);
	return 0; 

}
