#include "server.h"

int indexRoute(char requestBuffer[BUFSIZE], char pathBuffer[BUFSIZE], char contentBuffer[BUFSIZE]);
// i am going to make this easy and won't write a json parse so it will be just
// {taskname}
void tasks_post(char *contentBuffer, char *fullPath) {}

int routeRequest(char requestBuffer[BUFSIZE], char pathBuffer[BUFSIZE], char contentBuffer[BUFSIZE]) {

    if (strcmp(pathBuffer, "index.html")) {
        return indexRoute(requestBuffer, pathBuffer, contentBuffer);
    }
    return 0;
}

int indexRoute(char requestBuffer[BUFSIZE], char pathBuffer[BUFSIZE], char contentBuffer[BUFSIZE]) {
    if (requestBuffer[0] == 'P') {
        char bodyContent[BUFSIZE];
        char tempBuf[BUFSIZE];
        int contentLength = readFile(contentBuffer, pathBuffer);
        parseRequestBody(requestBuffer, bodyContent);
        sprintf(tempBuf, "- [ ] %s\n", bodyContent);

        strcpy(bodyContent, tempBuf);
        writeFile(bodyContent, "webserver/database.txt");
        return contentLength;
    }
    if (requestBuffer[0] == 'G') {
        int contentLength = readFile(contentBuffer, pathBuffer);
        parseRequestBody(requestBuffer, contentBuffer);
        return contentLength;
    }

    return 0;
}
