#include "server.h"

int handleGetRequest(httpM* response, httpM* request);
int tasksPost(httpM* response, httpM* request);
// i am going to make this easy and won't write a json parse so it will be just
// {taskname}

int routeRequest(httpM* response, httpM* request) {

    // handle GET
    if (request->method == 0) {
        handleGetRequest(response, request);
        // handle POST
    } else if (request->method == 1) {
        if (strcmp(request->path, "tasks")) {
            return tasksPost(response, request);
        }
    }

    return 0;
}

int tasksPost(httpM* response, httpM* request) {
    char tmp[BUFSIZE];
    snprintf(tmp, BUFSIZE - 8, "- [ ] %s\n", request->body);

    strcpy(request->body, tmp);
    writeFile(request->body, "webserver/database.txt");
    return 0;
}

int handleGetRequest(httpM* response, httpM* request) {
    // read file into body
    response->headers->contentLength = readFile(response->body, request->path);
    // getContentType
    parseContentTypeFromPath(response, request);
    // construct httpHeaders
    constructHttpHeaders(response, request);
    // copy body into message, headers should already be there
    strncat(response->message, response->body, BUFSIZE + HEADERBUFSIZE);

    return 1;
}
