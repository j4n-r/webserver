#include "server.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
void *handle_connection(void *client_socket);

typedef enum requestMethod {
    GET = 1,
    POST = 2,
    ERROR = 0,
} method;

typedef struct httpHeaders {
    char status[50];
    int contentLenght;
    char contentType[50];
    char date[50];
} httpH;

typedef struct httpMessage {
    method method;
    char headers[HEADERBUFSIZE];
    char body[BUFSIZE];
    char message[HEADERBUFSIZE + BUFSIZE];
} httpM;

int main(int argc, char **argv) {
    int server_socket = 0, client_socket = 0, addr_size = 0;
    SA_IN server_addr = {}, client_addr = {};

    // make a new socket with ipv4 and tcp
    checkErr((server_socket = socket(AF_INET, SOCK_STREAM, 0)),
             "Failed to create socket");

    // zero out struct and fill declare family as ipv4, serverport INADDR_ANY ??
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVERPORT);

    // bind socketto port
    checkErr(bind(server_socket, (SA *)&server_addr, sizeof(server_addr)),
             "Bind failed");
    // list on socket and declare how many connections can be in the queue
    checkErr(listen(server_socket, SERVER_BACKLOG), "Listen failed");

    // loop for new connections
    while (true) {
        printf("Waiting for connections ...\n");

        // get socklen for accept method
        addr_size = sizeof(SA_IN);
        // accept new connections and get back a new socket which with the new
        // connection, client_addr stays for new connections
        client_socket = accept(server_socket, (SA *)&client_addr, (socklen_t *)&addr_size);
        checkErr(client_socket, "accept failed");
        printf("Connected!\n");

        pthread_t newThread;
        int *pClientSocket = malloc(sizeof(int));
        *pClientSocket = client_socket;
        pthread_create(&newThread, NULL, handle_connection, pClientSocket);
        pthread_detach(newThread);
    }
}

void *handle_connection(void *pClientSocket) {
    int client_socket = *((int *)pClientSocket);
    free(pClientSocket);
    httpM request = {};
    httpM response = {};

    readAndPrintRequest(client_socket, requestBuffer);

    //  get file path from request (url)
    getPathFromRequest(requestBuffer, pathBuffer);
    routeRequest(requestBuffer, pathBuffer, contentBuffer);

    //  get content from file
    /*contentLength = readFile(contentBuffer, pathBuffer);*/

    // change this implementaition to something more structered

    parseContentTypeFromPath(pathBuffer, contentType);
    // construct http headers and get length of it
    size_t headerLength =
        constructHttpHeaders(responseBuffer, contentLength, contentType);

    // copy contenbuffer directly after the requestBuffer in memory
    memcpy(responseBuffer + headerLength, contentBuffer, contentLength);

    // send the responseBuffer to the socket
    send(client_socket, responseBuffer, headerLength + contentLength, 0);
    /*printf("*********************** Response *****************************\n
     * %s\n********************************************\n", responseBuffer);*/
    // close everything
    close(client_socket);
    printf("Connection close\n");
    return NULL;
}
