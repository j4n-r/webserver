/*
 * Thank you for the great videos https://youtube.com/@jacobsorber
 */

#ifndef SERVER_H
#define SERVER_H
#include <arpa/inet.h>
#include <bits/pthreadtypes.h>
#include <bits/types/struct_iovec.h>
#include <limits.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define SERVERPORT 8080
#define BUFSIZE 43276
#define HEADERBUFSIZE 4096
#define PATHBUFSIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 1

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

typedef enum requestMethod {
    GET = 0,
    POST = 1,
    ERROR = -1,
} method;

typedef struct httpHeaders {
    char status[50];
    int contentLength;
    char contentType[50];
    char date[50];
} httpH;

typedef struct httpMessage {
    method method;
    httpH* headers;
    char path[PATHBUFSIZE];
    char body[BUFSIZE];
    char message[HEADERBUFSIZE + BUFSIZE];
} httpM;

// if exp == -1 , print error message and qui programm
int checkErr(const int exp, const char* msg);
// puts the headers in the
size_t constructHttpHeaders(httpM* response, httpM* request);
size_t getRequest(httpM* request, const int client_socket);
int getPathFromRequest(httpM* request);
size_t readFile(char* contentBuffer, const char* fullPath);
int parseContentTypeFromPath(httpM* response, httpM* request);
size_t parseRequestBody(httpM* request);
size_t writeFile(char contentBuffer[BUFSIZE], const char* fullPath);
int routeRequest(httpM* response, httpM* request);
int getRequestMethod(httpM* request);
void printHttpRequest(const httpM* m);
void printHttpResponse(const httpM* m);
#endif // SERVER_H
