/*
 * Thank you for the great videos https://youtube.com/@jacobsorber
 */
#include <arpa/inet.h>
#include <bits/types/struct_iovec.h>
#include <limits.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define SERVERPORT 8080
#define BUFSIZE 432768
#define HEADERBUFSIZE 4096
#define PATHBUFSIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 1

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void handle_connection(int client_socket);
size_t constructHttpHeaders(char* headerBuffer, size_t contentLength, char* contentType);
int checkErr(int exp, const char* msg);
void printRequest(int client_socker, char* requestBuffer);
void getPathFromRequest(char* request, char* pathBuffer);

int main(int argc, char** argv) {
    int server_socket, client_socket, addr_size;
    SA_IN server_addr, client_addr;

    // make a new socket with ipv4 and tcp
    checkErr((server_socket = socket(AF_INET, SOCK_STREAM, 0)), "Failed to create socket");

    // zero out struct and fill declare family as ipv4, serverport INADDR_ANY ??
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVERPORT);

    // bind socketto port
    checkErr(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)), "Bind failed");
    // list on socket and declare how many connections can be in the queue
    checkErr(listen(server_socket, SERVER_BACKLOG), "Listen failed");

    // loop for new connections
    while (true) {
        printf("Waiting for connections ...\n");

        // get socklen for accept method
        addr_size = sizeof(SA_IN);
        // accept new connections and get back a new socket which with the new connection,
        // client_addr stays for new connections
        client_socket = accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size);
        checkErr(client_socket, "accept failed");
        printf("Connected!\n");

        handle_connection(client_socket);
    }
}

void handle_connection(int client_socket) {
    char responseBuffer[BUFSIZE + HEADERBUFSIZE] = {};
    char contentType[] = "text/html; charset=utf-8";
    char contentBuffer[BUFSIZE] = "Hello World";
    size_t contentLength = strlen(contentBuffer);
    char requestBuffer[BUFSIZE] = {};
    char pathBuffer[PATHBUFSIZE] = {};

    printRequest(client_socket, requestBuffer);

    // TODO get file path from request (url)
    getPathFromRequest(requestBuffer, pathBuffer);

    // TODO get content from file

    // construct http headers and get length of it
    size_t headerLength = constructHttpHeaders(responseBuffer, contentLength, contentType);

    // copy contenbuffer directly after the requestBuffer in memory
    memcpy(responseBuffer + headerLength, contentBuffer, contentLength);

    // send the responseBuffer to the socket
    send(client_socket, responseBuffer, headerLength + contentLength, 0);
    // close everything
    close(client_socket);
    printf("Connection close\n");
}

size_t constructHttpHeaders(char* contentBuffer, size_t contentLength, char* contentType) {
    time_t now = time(NULL);
    char date[128];
    strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&now));
    snprintf(contentBuffer, HEADERBUFSIZE, "HTTP/1.1 200 OK\r\n"
                                           "Date: %s\r\n"
                                           "Content-Length: %zu\r\n"
                                           "Content-Type: %s\r\n"
                                           "Connection: keep-alive\r\n"
                                           "\r\n",
             date, contentLength, contentType);
    return strlen(contentBuffer);
}

void getPathFromRequest(char* request, char* pathBuffer) {

    for (int i = 0, j = 0; i < PATHBUFSIZE; i++) {
        if (request[i] == '/') {
            while (request[i] != ' ') {
                pathBuffer[j++] = request[i++];
            }
            break;
        }
    }
    printf("Path: %s\n", pathBuffer);
}

void printRequest(int client_socket, char* requestBuffer) {
    checkErr(read(client_socket, requestBuffer, BUFSIZE), "Error on read request");
    printf("%s", requestBuffer);
}

int checkErr(int exp, const char* msg) {
    if (exp == -1) {
        perror(msg);
        exit(1);
    }
    return exp;
}
