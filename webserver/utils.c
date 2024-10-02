#include "server.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

size_t readFile(char *contentBuffer, const char *fullPath) {
    FILE *fp = fopen(fullPath, "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }
    size_t bytesRead = fread(contentBuffer, sizeof(char), BUFSIZE, fp);
    fclose(fp);
    return bytesRead;
}

size_t writeFile(char contentBuffer[BUFSIZE], const char *fullPath) {
    FILE *fp = fopen(fullPath, "a");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }
    size_t contentLength = strlen(contentBuffer);
    size_t bytesRead = fwrite(contentBuffer, sizeof(char), contentLength, fp);
    fclose(fp);
    return bytesRead;
}

void readAndPrintRequest(const int client_socket, char *requestBuffer) {

    size_t bytesRead = 0;
    checkErr(bytesRead = read(client_socket, requestBuffer, BUFSIZE - 1), "Error on read request");
    if (bytesRead > 0)
        requestBuffer[bytesRead] = '\0';
    else
        requestBuffer[0] = '\0';

    printf("*********************** REQUEST ***************************\n%s\n ********************************************\n", requestBuffer);
}

int checkErr(int exp, const char *msg) {
    if (exp == SOCKETERROR) {
        perror(msg);
        exit(1);
    }
    return exp;
}

void parseContentTypeFromPath(const char *path, char *contentType) {
    char extension[10] = {};
    char js[] = "text/javascript; charset=utf-8";
    char html[] = "text/html; charset=utf-8";
    char css[] = "text/css; charset=utf-8";

    for (int i = 0, j = 0; (size_t)i < strlen(path); i++) {
        if (path[i] == '.') {
            while (path[i] != '\0' && j < 9) {
                extension[j++] = path[++i];
            }
            extension[j] = '\0';
            break;
        }
    }
    if (!strcmp(extension, "js")) {
        strncpy(contentType, js, strlen(js));

    } else if (!strcmp(extension, "css")) {
        strncpy(contentType, css, strlen(css));

    } else if (!strcmp(extension, "html")) {
        strncpy(contentType, html, strlen(html));
    }
    printf("Extension: %s\n", extension);
    printf("Content type: %s\n", contentType);
}

void getPathFromRequest(const char *request, char *pathBuffer) {
    char relativPathBuffer[PATHBUFSIZE] = {};
    char *relativPath = "frontend/";
    memcpy(relativPathBuffer, relativPath, strlen(relativPath));

    for (int i = 0, j = strlen(relativPathBuffer); i < PATHBUFSIZE; i++) {
        if (request[i] == '/') {
            while (request[i] != ' ' && j < BUFSIZE) {
                relativPathBuffer[j++] = request[i++];
            }
            break;
        }
    }
    if (realpath(relativPathBuffer, pathBuffer) == NULL) {
        perror("error resolving path");
        exit(1);
    }
    // TODO add checking for wrong direcotry
    printf("Relativ path: %s\n", relativPathBuffer);
    printf("Full path: %s\n", pathBuffer);
}

size_t constructHttpHeaders(char *contentBuffer, const size_t contentLength, const char *contentType) {
    time_t now = time(NULL);
    char date[128];
    if (strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&now)) == 0)
        exit(1);
    snprintf(contentBuffer, HEADERBUFSIZE, "HTTP/1.1 200 OK\r\n"
                                           "Date: %s\r\n"
                                           "Content-Length: %zu\r\n"
                                           "Content-Type: %s\r\n"
                                           "Connection: keep-alive\r\n"
                                           "\r\n",
             date, contentLength, contentType);

    return strlen(contentBuffer);
}

size_t parseRequestBody(char *requestBuffer, char bodyContent[BUFSIZE]) {
    char *bodyStart = strstr(requestBuffer, "{");
    char *bodyEnd = strstr(requestBuffer, "}");
    if (bodyStart == NULL || bodyEnd == NULL) {
        printf("No body found");
        return (size_t)NULL;
    } else {

        char content[BUFSIZE] = {};
        for (int i = 0; bodyStart != bodyEnd - 1 || i == BUFSIZE; i++) {
            content[i] = *++bodyStart;
        }
        strcpy(bodyContent, content);
        return strlen(bodyContent);
    }
}
