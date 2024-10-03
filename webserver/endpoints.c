#include "server.h"

int indexRoute(httpM* response, httpM* request);
// i am going to make this easy and won't write a json parse so it will be just
// {taskname}
void tasks_post(char* contentBuffer, char* fullPath) {}

int routeRequest(httpM* response, httpM* request) {

    if (strcmp(request->path, "index.html")) {
        return indexRoute(response, request);
    }
    return 0;
}

int indexRoute(httpM* response, httpM* request) {
    char tmp[BUFSIZE];
    if (request->method == 1) {
        int contentLength = readFile(response->body, request->path);
        sprintf(tmp, "- [ ] %s\n", response->body);

        strcpy(response->body, tmp);
        writeFile(response->body, "webserver/database.txt");
        return contentLength;
    }
    if (request->method == 0) {
        int contentLength = readFile(response->body, request->path);
        return contentLength;
    }

    return 0;
}
