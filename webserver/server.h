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
#define BUFSIZE 432768
#define HEADERBUFSIZE 4096
#define PATHBUFSIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 1

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

int checkErr(const int exp,const char* msg);
size_t constructHttpHeaders(char* headerBuffer, size_t contentLength, const char* contentType);
void printRequest(const int client_socket, char* requestBuffer);
void getPathFromRequest(const char* request, char* pathBuffer);
size_t readFile(char* contentBuffer, const char* fullPath);
void parseContentTypeFromPath(const char* path, char* contentType);

#endif  // SERVER_H
