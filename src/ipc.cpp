#include <iostream>
#include "../include/IPCLib/ipc.h"
#include <unistd.h>
#include <sys/socket.h>

#define BUFFER_SIZE 128

//TODO Replace character pointers to strings
int server::bindSocket(struct sockaddr_un * sock , char const * sockFile){

    int ret;
    int connection_socket;

    /* In case program exiteded inadvertently on last run
     * remove socket */
    unlink(sockFile);
    /* Create Master Socket */
    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(connection_socket == -1){
        perror("socket");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket was created successfully" << std::endl;

    /*Initialize socket*/
    memset(sock, 0, sizeof(struct sockaddr_un));

    /*Specify socket credentials*/
    sock -> sun_family = AF_UNIX;
    strncpy(sock->sun_path, sockFile, sizeof(sock->sun_path) - 1);

    /*Bind syscall*/
    ret = bind(connection_socket, 
            (const struct sockaddr *) sock, 
            sizeof(sockaddr_un));
    if(ret == -1){
        perror("bind");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket successfully binded with status: " 
        << ret 
        << std::endl;

    /* Return master file descriptor */
    return connection_socket;
}

int server::serverListen(int socketFD, int maxConnections){
    int ret;

    /* Prepare for accepting connections, with backlog size 20 */
    ret = listen(socketFD, maxConnections);
    if(ret == -1){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int server::serverAccept(int socketFD, char * dataBuffer){
    int ret;
    int dataSocket; // Carries out actual data exchange with client
    /* Accept syscall and initialize data file descriptor */
    dataSocket = accept(socketFD, NULL, NULL);
    if(dataSocket == -1){
        perror("accept");
        exit(EXIT_FAILURE);
    }
    std::cout << "Connection accepted from client" << std::endl;

    /* Prepare buffer */
    memset(dataBuffer, 0, BUFFER_SIZE);

    /* Wait for next data packet
     * Block server process. Wait for data to arrive from client */
    std::cout << "Waiting for data from client" << std::endl;
    ret = read(dataSocket, dataBuffer, BUFFER_SIZE);
    if(ret == -1){
        perror("read");
        exit(EXIT_FAILURE);
    }

    /* Send data back to client */
    printf("%s\n", dataBuffer);
    std::cout << "Sending response back to client" << std::endl;
    ret = write(dataSocket, dataBuffer, BUFFER_SIZE);
    if(ret == -1){
        perror("write");
        exit(EXIT_FAILURE);
    }
    /* Close Socket */
    close(dataSocket);
    /* Close Master Socket */
    close(socketFD);
    /* Server should release resources before getting terminated */
    // unlink(SOCKET_NAME);
    
    /* Server operations executed successfully */
    return 0;
}


int client::createSocket(struct sockaddr_un * sock , char const * sockFile){
    int dataSocket;
    int ret;

    /* Create data socket */
    dataSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(dataSocket == -1){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Clear whole structure and populate standard fields */
    memset(sock, 0, sizeof(struct sockaddr_un));

    /* Connect to socket address. Non-blocking syscall */
    sock -> sun_family = AF_UNIX;
    strncpy(sock->sun_path, sockFile, sizeof(sock->sun_path) - 1);
    ret = connect(dataSocket, 
            (const struct sockaddr *) sock, 
            sizeof(sockaddr_un));
    if(ret ==-1){
        fprintf(stderr, "The server is down.\n");
        exit(EXIT_FAILURE);
    }
    return dataSocket;
}

int client::send(int dataSocket , char * dataBuffer){
    int ret;

    /* Send data to server from buffer */
    ret = write(dataSocket, dataBuffer, BUFFER_SIZE );
    if(ret == -1){
        perror("write");
        exit(EXIT_FAILURE);
    }

    /* Request data from server */
    memset(dataBuffer, 0, BUFFER_SIZE);
    ret = read(dataSocket, dataBuffer, BUFFER_SIZE ); 
    if(ret == -1){
        perror("read");
        exit(EXIT_FAILURE);
    }
    std::cout << "Recvd from Server: " << dataBuffer << std::endl;

    /* Close socket */
    close(dataSocket);
    return 0;
}

