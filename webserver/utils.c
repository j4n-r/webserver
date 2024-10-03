#include "server.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

size_t readFile(char* contentBuffer, const char* fullPath) {
    FILE* fp = fopen(fullPath, "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }
    size_t bytesRead = fread(contentBuffer, sizeof(char), BUFSIZE, fp);
    fclose(fp);
    return bytesRead;
}

size_t writeFile(char contentBuffer[BUFSIZE], const char* fullPath) {
    FILE* fp = fopen(fullPath, "a");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }
    size_t contentLength = strlen(contentBuffer);
    size_t bytesRead = fwrite(contentBuffer, sizeof(char), contentLength, fp);
    fclose(fp);
    return bytesRead;
}

size_t getRequest(httpM* request, const int client_socket) {

    size_t bytesRead = 0;
    checkErr(bytesRead = read(client_socket, request->message, BUFSIZE - 1),
             "Error on read request");
    if (bytesRead > 0)
        request->message[bytesRead] = '\0';
    else
        request->message[0] = '\0';
    return bytesRead;
}

int checkErr(int exp, const char* msg) {
    if (exp == SOCKETERROR) {
        perror(msg);
        exit(1);
    }
    return exp;
}

void parseContentTypeFromPath(const char* path, char* contentType) {
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

int getPathFromRequest(httpM* request) {
    char* dir = "frontend";
    char relativePath[PATHBUFSIZE] = {};
    strncpy(relativePath, dir, strlen(dir));

    for (int i = 0, j = strlen(dir); i < PATHBUFSIZE; i++) {
        if (request->message[i] == '/') {
            while (request->message[i] != ' ' && j < PATHBUFSIZE) {
                relativePath[j++] = request->message[i++];
            }
            break;
        }
        relativePath[strlen(relativePath) + 1] = '0';
    }
    if (realpath(relativePath, request->path) == NULL) {
        perror("Error resolving path");
        return -1;
    }
    return 1;
}

size_t constructHttpHeaders(char* contentBuffer, const size_t contentLength,
                            const char* contentType) {
    time_t now = time(NULL);
    char date[128];
    if (strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT",
                 gmtime(&now)) == 0)
        exit(1);
    snprintf(contentBuffer, HEADERBUFSIZE,
             "HTTP/1.1 200 OK\r\n"
             "Date: %s\r\n"
             "Content-Length: %zu\r\n"
             "Content-Type: %s\r\n"
             "Connection: keep-alive\r\n"
             "\r\n",
             date, contentLength, contentType);

    return strlen(contentBuffer);
}

size_t parseRequestBody(httpM* request) {
    char* bodyStart = strstr(request->message, "{");
    char* bodyEnd = strstr(request->message, "}");
    if (bodyStart == NULL || bodyEnd == NULL) {
        printf("No body found");
        return (size_t)NULL;
    } else {

        for (int i = 0; bodyStart != bodyEnd - 1 || i == BUFSIZE; i++) {
            request->body[i] = *++bodyStart;
        }
        return strlen(request->body);
    }
}

int getRequestMethod(httpM* request) {

    if (request->message[0] == 'G') {
        request->method = 0;
    } else if (request->message[0] == 'P') {
        request->method = 1;
    } else {
        return -1;
    }

    return 1;
}

void printHttpMessage(const httpM* m) {
    printf("############################## START REQUEST "
           "#################################################\n"
           "Method: %d\n"
           "Headers:\n"
           "Status: %s\n"
           "ContentLength: %d\n"
           "ContentType: %s\n"
           "Date: %s\n"
           "Path: %s\n"
           "body:\n%s\n"
           "message:\n%s\n"
           "############################## END REQUEST "
           "########################################## \n",
           m->method, m->headers->status, m->headers->contentLength,
           m->headers->contentType, m->headers->date, m->path, m->body,
           m->message);
}
