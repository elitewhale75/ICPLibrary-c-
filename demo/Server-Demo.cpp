#include <ipclib/ipc.h>
#include <iostream>
#include <unistd.h>

#define SOCK_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128


int main () {
    const int MAX_CONNECTIONS = 3;
    int masterSocket;
    int dataFile;
    int ret;
    sockaddr_un sock;
    char * dataBuffer = (char *) malloc(255);

    // Create Master Socket
    masterSocket = server::bindSocket(&sock, SOCK_NAME);

    // Open server process to 3 client proccesses
    server::serverListen(masterSocket, MAX_CONNECTIONS);

    for (;;){
        // Block server process and wait for requests from client processes
        dataFile = server::serverAccept(masterSocket);

        // Service client request
        memset(dataBuffer, 0, BUFFER_SIZE);
        ret = read(dataFile, dataBuffer, BUFFER_SIZE);
        if(ret == -1){
            perror("read");
            exit(EXIT_FAILURE);
        }

        // Perform operations replace Ls with Ws
        char * current;
        for ( int i = 0 ; *(dataBuffer + i) ; i++ ) {
            current = dataBuffer + i;
            if( *current == 'l' )
                *current = 'w';
        }

        // Send message back to client process
        ret = write(dataFile, dataBuffer, BUFFER_SIZE);
        if(ret == -1){
            perror("write");
            exit(EXIT_FAILURE);
        }

    }

    return 0;
}