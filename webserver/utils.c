#include "server.h"
#include <stdio.h>

size_t readFile(char* contentBuffer, const char* fullPath) {
    FILE* fp = fopen(fullPath, "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }
    return fread(contentBuffer, sizeof(char), BUFSIZE, fp);
}

void printRequest(const int client_socket, char* requestBuffer) {

    size_t bytesRead = 0;
    checkErr(bytesRead = read(client_socket, requestBuffer, BUFSIZE - 1), "Error on read request");
    if (bytesRead > 0)
        requestBuffer[bytesRead] = '\0';
    else
        requestBuffer[0] = '\0';

    printf("*********************** REQUEST ***************************\n%s\n ********************************************\n", requestBuffer);
}

int checkErr(int exp, const char* msg) {
    if (exp == SOCKETERROR) {
        perror(msg);
        exit(1);
    }
    return exp;
}
