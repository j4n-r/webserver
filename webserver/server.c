#include "server.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>

void* handle_connection(void* client_socket);
int handleRequest(httpM* request, int socket);
int handleResponse(httpM* response, httpM* request);

int main(int argc, char** argv) {
    int server_socket = 0, client_socket = 0, addr_size = 0;
    SA_IN server_addr = {}, client_addr = {};

    // make a new socket with ipv4 and tcp
    checkErr((server_socket = socket(AF_INET, SOCK_STREAM, 0)),
             "Failed to create socket");

    // declare family as ipv4, serverport INADDR_ANY ??
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // bind socketto port
    for (int port = SERVERPORT; port < 64000; port++) {
        server_addr.sin_port = htons(port);
        if (!((bind(server_socket, (SA*)&server_addr, sizeof(server_addr))) ==
              -1)) {
            printf("Port: %d ", port);
            break;
        }
    }
    // list on socket and declare how many connections can be in the queue
    checkErr(listen(server_socket, SERVER_BACKLOG), "Listen failed");

    // loop for new connections
    while (true) {
        printf("Waiting for connections...\n");

        // get socklen for accept method
        addr_size = sizeof(SA_IN);
        // accept new connections and get back a new socket which with the new
        // connection, client_addr stays for new connections
        client_socket =
            accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size);
        checkErr(client_socket, "accept failed");
        printf("Connected!\n");

        pthread_t newThread;
        int* pClientSocket = malloc(sizeof(int));
        *pClientSocket = client_socket;
        pthread_create(&newThread, NULL, handle_connection, pClientSocket);
        pthread_detach(newThread);
    }
}

void* handle_connection(void* pClientSocket) {
    int client_socket = *((int*)pClientSocket);
    free(pClientSocket);
    httpM request = {};
    httpM response = {};
    request.headers = &(httpH){};
    response.headers = &(httpH){};

    // fill in request
    checkErr(handleRequest(&request, client_socket), "Handling request failed");

    printHttpRequest(&request);
    // fill in the response based on the tyupe of request and path
    handleResponse(&response, &request);

    // send response
    send(client_socket, response.message, strlen(response.message), 0);

    // close everything
    close(client_socket);
    printf("Connection close\n");
    return NULL;
}

int handleRequest(httpM* request, const int socket) {
    // copy request into request
    if (!(getRequest(request, socket) > 0)) {
        printf("Empty request");
        return 0;
    }
    // extract path from request
    getPathFromRequest(request);
    checkErr(getRequestMethod(request), "invalid request");
    printHttpRequest(request);
    parseRequestBody(request);
    return 1;
}

int handleResponse(httpM* response, httpM* request) {
    // fills in the response body
    routeRequest(response, request);
    printHttpResponse(response);
    return 1;
}
