#include "server.h"

int getTasks(httpM* response, httpM* request);
int handleGetRequest(httpM* response, httpM* request);
int postTask(httpM* response, httpM* request);
// i am going to make this easy and won't write a json parse so it will be just
// {taskname}

int routeRequest(httpM* response, httpM* request) {

    // handle GET
    if (request->method == 0) {
        if (strncmp(request->path, "getTasks", PATHBUFSIZE)) {
            getTasks(response, request);
        }
        handleGetRequest(response, request);
        // handle POST
    } else if (request->method == 1) {
        if (strcmp(request->path, "tasks")) {
            return postTask(response, request);
        }
    }

    return 0;
}

int getTasks(httpM* response, httpM* request) {
    int bytesRead = readFile(response->body, DATABASE_URL);
    printf("Butes read: %d\n", bytesRead);
    for (int i = 0; i < strlen(response->body); i++) {
        if (response->body[i] == '-') {
            response->body[i] = ';';
            continue;
        }
        if (response->body[i] == '[') {
            response->body[i] = ' ';
            continue;
        }
        if (response->body[i] == ']') {
            response->body[i] = ' ';
            continue;
        }
    }

    parseContentTypeFromPath(response, request);
    response->method = POST;
    strcpy(response->path, "/getTasks");
    // construct httpHeaders
    fillReqHeaders(response, request);
    // copy body into message, headers should already be there
    strncat(response->message, response->body, BUFSIZE + HEADERBUFSIZE);
    return 1;
}

int postTask(httpM* response, httpM* request) {
    char tmp[BUFSIZE];
    snprintf(tmp, BUFSIZE - 8, "- [ ] %s\n", request->body);

    strcpy(request->body, tmp);
    writeFile(request->body, DATABASE_URL);
    return 0;
}

int handleGetRequest(httpM* response, httpM* request) {
    // read file into body
    response->headers->contentLength = readFile(response->body, request->path);
    // getContentType
    parseContentTypeFromPath(response, request);
    // construct httpHeaders
    fillResHeaders(response, request);
    // copy body into message, headers should already be there
    strncat(response->message, response->body, BUFSIZE + HEADERBUFSIZE);

    return 1;
}
